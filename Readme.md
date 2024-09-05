# Cross-Platform File Transfer

This project implements a **client-server** application in C++ using **Boost.Asio** for network communication.
The client transfers a file over the network to the server, which receives the file and stores it locally.

## Features

- The server listens for incoming file transfers and saves the received file.
- The client sends a specified file to the server.
- Cross-platform: Works on both Linux and Windows.
- Built using **Boost.Asio** for networking and **CMake** for building.

## Requirements

Before you begin, ensure that you have the following installed on your machine:

- **CMake** (version 3.10 or higher)
- **Boost** (with components: `system`, `thread`, `filesystem`)
- **A C++17-compatible compiler** (e.g., GCC, Clang, MSVC)

## Building the Project

1. Clone or download the project files to your local machine.

2. Create a build directory inside the project root:
```bash
mkdir build
cd build
```

3. Run CMake to generate the build files:
```bash
cmake ..
```
If Boost cannot be found dynamically at this stage, go to CMakeLists.txt and edit line number 14 with your boost's directory location.

4. Build the project:
```bash
cmake --build .
```

## Running the App
1. Run the server

```bash
./server <port>

eg.

./server 12345
```

Server will listen on this port waiting for a file sent by the client.

2. Run the client

```bash
./client <pathtofile> <serveraddress> <port>

eg.

./client.exe "C:\Users\x\Desktop\Capture.png" localhost 12345

or

./client.exe "C:\Users\x\Desktop\file.txt" 12.345.678.123 12345
'''

Client will connect to this server, send file and await for confirmation for server and then close the communication.

## Notes
* Localhost will work in most cases, for sending files via network you should have some basic understanding of networking.

* Ensure the server is running before executing the client.

* The client and server must use the same port for communication.

* If running the client and server on different machines, use the server’s IP address in the client command instead of localhost.