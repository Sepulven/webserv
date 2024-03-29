#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <poll.h>

#define MAX_CLIENTS 10

int main() {
    int server_socket, client_sockets[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024];

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating socket\n";
        exit(EXIT_FAILURE);
    }

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // Bind server socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed\n";
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    listen(server_socket, 3);
    std::cout << "Server listening...\n";

    // Initialize pollfd structure for server socket
    struct pollfd fds[MAX_CLIENTS + 1];
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;

    // Initialize client sockets array
    memset(client_sockets, 0, sizeof(client_sockets));

    while (true) {
        // Wait for events
        int activity = poll(fds, MAX_CLIENTS + 1, -1);

        if (activity < 0) {
            std::cerr << "Poll error\n";
            exit(EXIT_FAILURE);
        }

        // If server socket has incoming connection
        if (fds[0].revents & POLLIN) {
            int new_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t*)&client_addr);
            if (new_socket < 0) {
                std::cerr << "Accept failed\n";
                exit(EXIT_FAILURE);
            }
            std::cout << "New connection, socket fd: " << new_socket << ", IP: " << inet_ntoa(client_addr.sin_addr) << ", Port: " << ntohs(client_addr.sin_port) << std::endl;

            // Add new socket to fds array
            for (int i = 1; i < MAX_CLIENTS + 1; ++i) {
                if (client_sockets[i] == 0) {
                    fds[i].fd = new_socket;
                    fds[i].events = POLLIN;
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        // Check for data from clients
        for (int i = 1; i < MAX_CLIENTS + 1; ++i) {
            if (client_sockets[i] > 0 && fds[i].revents & POLLIN) {
                int valread = read(client_sockets[i], buffer, sizeof(buffer));
                if (valread == 0) {
                    // Client disconnected
                    close(client_sockets[i]);
                    client_sockets[i] = 0;
                    fds[i].fd = -1;
                    std::cout << "Client disconnected\n";
                } else {
                    buffer[valread] = '\0';
                    std::cout << "Message from client " << i << ": " << buffer << std::endl;
                }
            }
        }
    }

    // Close server socket
    close(server_socket);

    return 0;
}
