#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

// Global variable for the static directory path
fs::path static_dir;

// Helper function to read a file into a string
std::string read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Helper function to get content type based on file extension
std::string get_content_type(const std::string& path) {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return "text/plain";
    }
    
    std::string ext = path.substr(dot_pos);
    
    if (ext == ".html") return "text/html";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "text/javascript";
    return "text/plain";
}

// Function to handle HTTP request and generate response
std::string handle_request(const std::string& request) {
    std::istringstream request_stream(request);
    std::string method, path, protocol;
    request_stream >> method >> path >> protocol;

    std::cout << "Received request: Method=" << method 
              << ", Path=" << path 
              << ", Protocol=" << protocol << std::endl;

    if (path == "/") path = "/index.html";
    
    fs::path filepath = static_dir / path.substr(1);  // Remove leading '/'
    
    std::cout << "Looking for file: " << filepath << std::endl;

    // Check if file exists and is within static directory
    if (!fs::exists(filepath) || !fs::is_regular_file(filepath)) {
        std::cerr << "File not found: " << filepath << std::endl;
        return "HTTP/1.1 404 Not Found\r\n"
               "Content-Length: 9\r\n"
               "Content-Type: text/plain\r\n"
               "\r\n"
               "Not Found";
    }

    // Read the file
    std::string content = read_file(filepath.string());
    if (content.empty()) {
        std::cerr << "File is empty or couldn't be read: " << filepath << std::endl;
        return "HTTP/1.1 500 Internal Server Error\r\n"
               "Content-Length: 21\r\n"
               "Content-Type: text/plain\r\n"
               "\r\n"
               "Internal Server Error";
    }

    // Create response
    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: " << get_content_type(filepath.string()) << "\r\n"
             << "Content-Length: " << content.length() << "\r\n"
             << "\r\n"
             << content;

    std::cout << "Sending response with content type: " 
              << get_content_type(filepath.string()) << std::endl;

    return response.str();
}

int main(int argc, char* argv[]) {
    // Set up static directory path
    fs::path exec_path = fs::absolute(fs::path(argv[0]));
    static_dir = exec_path.parent_path() / "../static";  // Changed this line
    
    std::cout << "Static directory path: " << static_dir << std::endl;
    
    // Convert to canonical path to resolve the "../static" properly
    try {
        static_dir = fs::canonical(static_dir);  // Added this line
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error resolving static directory path: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Resolved static directory path: " << static_dir << std::endl;
    
    if (!fs::exists(static_dir) || !fs::is_directory(static_dir)) {
        std::cerr << "Static directory not found at: " << static_dir << std::endl;
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }
    std::cout << "Socket created successfully\n";

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3000);       
    addr.sin_addr.s_addr = INADDR_ANY;

    // Add this to allow port reuse
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "setsockopt failed\n";
        return 1;
    }

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cerr << "Failed to bind to port 3000\n";
        return 1;
    }
    std::cout << "Bound to port 3000 successfully\n";

    if (listen(sockfd, 10) == -1) {
        std::cerr << "Failed to listen on socket\n";
        return 1;
    }
    std::cout << "Server is listening on port 3000...\n";

    while (true) {
        std::cout << "Waiting for connection...\n";
        int client_sockfd = accept(sockfd, nullptr, nullptr);
        if (client_sockfd == -1) {
            std::cerr << "Failed to accept connection\n";
            continue;
        }
        std::cout << "Connection accepted!\n";

        char buffer[4096] = {0};
        ssize_t bytes_read = read(client_sockfd, buffer, 4096);
        if (bytes_read > 0) {
            std::string request(buffer, bytes_read);
            std::string response = handle_request(request);
            write(client_sockfd, response.c_str(), response.length());
        }

        close(client_sockfd);
    }
    return 0;
}
