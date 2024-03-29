/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receiver.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 17:00:52 by asepulve          #+#    #+#             */
/*   Updated: 2024/03/29 17:02:37 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024];

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating socket\n";
        exit(EXIT_FAILURE);
    }

    // Bind socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080); // Port number
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed\n";
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    listen(server_socket, 3);
    std::cout << "Server listening...\n";

    // Accept incoming connection
    socklen_t addr_size = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
    if (client_socket < 0) {
        std::cerr << "Accept failed\n";
        exit(EXIT_FAILURE);
    }
    std::cout << "Client connected\n";

    // Receive data from client
    recv(client_socket, buffer, sizeof(buffer), 0);
    std::cout << "Message from client: " << buffer << std::endl;

    // Close sockets
    close(client_socket);
    close(server_socket);

    return 0;
}
