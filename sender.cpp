#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char message[] = "Hello from client";

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error creating socket\n";
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080); // Port number
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed\n";
        exit(EXIT_FAILURE);
    }

    // Send data to server
    send(client_socket, message, strlen(message), 0);
    std::cout << "Message sent to server\n";

    // Close socket
    close(client_socket);

    return 0;
}
