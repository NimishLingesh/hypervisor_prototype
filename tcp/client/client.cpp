#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main() {
    // create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    // specify the server address and port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);
    
    
    // connect to the server
    connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    
    // receive data from the server
    char buffer[1024] = {0};
    read(client_socket, buffer, 1024);
    std::cout << "Server message: " << buffer << std::endl;
    
    // send data to the server
    const char *message = "Hello, server!";
    send(client_socket, message, strlen(message), 0);
    
    // close the socket
    close(client_socket);
    return 0;
}