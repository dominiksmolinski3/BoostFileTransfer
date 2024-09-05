#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <system_error>
#include <string>

using boost::asio::ip::tcp;

void receive_file(tcp::socket& socket) {
    boost::asio::streambuf buffer;
    std::istream stream(&buffer);
    boost::system::error_code error;
    std::ofstream outfile;

    try {
        // read filename
        boost::asio::read_until(socket, buffer, "\n");
        std::string header;
        std::getline(stream, header);
        std::string filename = header.substr(9);  // Skip "FILENAME:"

        // read file size
        boost::asio::read_until(socket, buffer, "\n");
        std::getline(stream, header);
        std::size_t file_size = std::stoul(header.substr(5));  // Skip "SIZE:"

        std::cout << "Expecting file of size: " << file_size << " bytes" << std::endl;

        // open output file for writing
        outfile.open(filename, std::ios::binary);
        if (!outfile.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return;
        }

        // receive file content
        std::size_t total_bytes_read = 0;
        char file_buffer[1024];

        while (total_bytes_read < file_size) {
            std::size_t bytes_left = file_size - total_bytes_read;
            std::size_t bytes_to_read = std::min(bytes_left, sizeof(file_buffer));

            std::size_t bytes_read = socket.read_some(boost::asio::buffer(file_buffer, bytes_to_read), error);

            if (error == boost::asio::error::eof) {
                // client closed the connection unexpectedly
                std::cerr << "Client closed the connection unexpectedly." << std::endl;
                break;
            } else if (error) {
                throw boost::system::system_error(error);
            }

            outfile.write(file_buffer, bytes_read);
            total_bytes_read += bytes_read;

            std::cout << "Received " << total_bytes_read << "/" << file_size << " bytes." << std::endl;
        }   


        std::cout << "File received successfully: " << filename << std::endl;

        // send acknowledgment to the client
        std::string acknowledgment = "ACK: File received successfully\n";
        boost::asio::write(socket, boost::asio::buffer(acknowledgment));

    } catch (boost::system::system_error& e) {
        std::cerr << "Error receiving file: " << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    // ensure file is closed
    if (outfile.is_open()) {
        outfile.close();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: server <port>" << std::endl;
        return 1;
    }

    unsigned short port = std::stoi(argv[1]);

    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
        std::cout << "Server listening on port " << port << std::endl;

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::cout << "Connection established. Receiving file..." << std::endl;
            receive_file(socket);

            std::cout << "Ready to accept the next file..." << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
