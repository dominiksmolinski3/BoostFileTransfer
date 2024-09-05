#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <system_error>

using boost::asio::ip::tcp;

void send_file(const std::string &filename, const std::string &server_address, const std::string &server_port) {
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints = resolver.resolve(server_address, server_port);
    tcp::socket socket(io_context);

    // Connect to the server with retries
    bool connected = false;
    for (int attempt = 0; attempt < 5 && !connected; ++attempt) {
        try {
            boost::asio::connect(socket, endpoints);
            connected = true;
        } catch (const std::exception &e) {
            std::cerr << "Connection attempt " << (attempt + 1) << " failed: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }

    if (!connected) {
        std::cerr << "Failed to connect to server after multiple attempts." << std::endl;
        return;
    }

    // Open the file for reading
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }

    // Get the file size
    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Send the filename
    std::string header = "FILENAME:" + boost::filesystem::path(filename).filename().string() + "\n";
    boost::asio::write(socket, boost::asio::buffer(header));

    // Send the file size
    std::string size_header = "SIZE:" + std::to_string(file_size) + "\n";
    boost::asio::write(socket, boost::asio::buffer(size_header));

    // Send the file content in chunks
    char buffer[1024];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        boost::asio::write(socket, boost::asio::buffer(buffer, file.gcount()));
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // To avoid overwhelming the server
    }
    socket.shutdown(tcp::socket::shutdown_send);
    std::cout << "File transfer complete! Waiting for server acknowledgment..." << std::endl;

    // Wait for acknowledgment from the server
    char ack_buffer[1024];
    boost::system::error_code error;
    std::size_t ack_length = socket.read_some(boost::asio::buffer(ack_buffer), error);

    if (error == boost::asio::error::eof) {
        std::cerr << "Server closed the connection unexpectedly." << std::endl;
    } else if (!error) {
        std::string ack_message(ack_buffer, ack_length);
        std::cout << "Received acknowledgment from server: " << ack_message << std::endl;
    } else {
        std::cerr << "Error receiving acknowledgment: " << error.message() << std::endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: client <filename> <server_address> <server_port>" << std::endl;
        return 1;
    }

    try {
        std::string filename = argv[1];
        std::string server_address = argv[2];
        std::string server_port = argv[3];

        send_file(filename, server_address, server_port);
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
