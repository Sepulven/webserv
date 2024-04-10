/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 13:22:29 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/10 13:20:34 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server() : domain(AF_INET), port(8080)
{
	this->max_events = 200;

	this->fd = socket(this->domain, SOCK_STREAM, 0);
	if (this->fd < 0)
		throw Server::Error("Socket failed.");
	if (Server::sfd_non_blocking(this->fd))
		throw Server::Error("Couldn't make the server socket non-blocking.");
	memset(&this->server_addr, 0, sizeof(struct sockaddr_in));
	this->server_addr.sin_family = this->domain;
	this->server_addr.sin_addr.s_addr = INADDR_ANY;
	this->server_addr.sin_port = htons(this->port);

	//We ensure that our vector won't change its area.
	this->connections.reserve(this->max_events);

	int enable = 1;
	if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw Server::Error("Setsockopt failed");
}

Server::~Server()
{
	close(this->fd);
}

void Server::accept_connection(int epoll_fd)
{
	struct epoll_event event;

	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(this->fd, (struct sockaddr *)&client_addr, &client_addr_len);

	if (client_fd < 0) 
		throw Server::Error("accept failed.");

	if (sfd_non_blocking(client_fd) < 0)
		throw Server::Error("Couln't make socket fd non-blocking.");
	event.events = EPOLLIN;
	event.data.fd = client_fd;

	this->connections.push_back(event);
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) < 0)
		throw Server::Error("Epoll_ctl failed");
}

void Server::read_request(int epoll_fd, struct epoll_event conn, int i)
{
	char buffer[1024];
	// handle_client request
	read(conn.data.fd, buffer, 1024);
	std::cout << buffer << std::endl;

	char buffer1[1024] = "HTTP/1.1 200 OK\n"
						"Date: Mon, 27 Jul 2009 12:28:53 GMT\n"
						"Server: Apache/2.2.14 (Win32)\n"
						"Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n"
						"Content-Length: 88\n"
						"Content-Type: text/html\n"
						"Connection: Closed\n"
						"\n"
						"<html>\n"
						"<body>\n"
						"<h1>Hello, World!</h1>\n"
						"</body>\n"
						"</html>\n"
						"\r\n";
	
	write(conn.data.fd, buffer1, sizeof(buffer1));

	// Remove it from the list of watched elements
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn.data.fd, NULL) < 0)
		throw Server::Error("Epoll_ctl failed here");

	// Close the connection and finishes the request
	close(conn.data.fd);

	// We must erase the element without changing the order of the elements.
	// So epoll could work

	this->connections.erase(this->connections.begin() + i);
}

void Server::listen(void)
{
		struct epoll_event event;
		int num_connections = 0;
		int epoll_fd = 0; 
		struct epoll_event current_connection;

		epoll_fd = epoll_create1(0);

		if (bind(this->fd, (struct sockaddr *)&this->server_addr, sizeof(server_addr)) < 0)
			throw Server::Error("Bind failed.");
		if (::listen(this->fd, this->max_events) < 0)
			throw Server::Error("Listen failed.");

		if (epoll_fd < 0)
			throw Server::Error("Epoll_create1 failed.");

		if (Server::sfd_non_blocking(epoll_fd) < 0)
			throw Server::Error("Couln't make epoll fd non-blocking.");

		event.events = EPOLLIN;
		event.data.fd = this->fd;
		
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, this->fd, &event))
			throw Server::Error("epoll_ctl failed.");
		while (true)
		{
			num_connections = epoll_wait(epoll_fd, this->connections.data(), this->max_events, -1);
			if (num_connections < 0)
				throw Server::Error("Epoll_wait failed.");
			for (int i = 0; i < num_connections; i++)
			{
				current_connection = this->connections[i];
				if (current_connection.events & EPOLLIN)
					std::cout << "EPOLLIN in " << current_connection.data.fd << std::endl;
				if ((current_connection.data.fd == this->fd) && (current_connection.events & EPOLLIN))
					accept_connection(epoll_fd);
				else if (this->connections[i].events & EPOLLIN)
					read_request(epoll_fd, current_connection, i);
			}
		}
}

/*Exception class*/
Server::Error::Error(const char *_msg) : msg(_msg) {}

const char * Server::Error::what() const throw()
{
    return (msg);
}
	