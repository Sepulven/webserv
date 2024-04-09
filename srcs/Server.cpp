/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 13:22:29 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/09 15:29:40 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server() : domain(AF_INET), port(8080)
{
	this->max_events = 20;

	this->fd = socket(this->domain, SOCK_STREAM, 0);
	if (this->fd < 0)
		throw Server::Error("Socket failed.");

	this->server_addr.sin_family = this->domain;
	this->server_addr.sin_addr.s_addr = INADDR_ANY;
	this->server_addr.sin_port = htons(8080);
	
	int enable = 1;
	if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw Server::Error("Setsockopt failed");
}

Server::~Server()
{
	close(this->fd);
}

void accept_connection(int epoll_fd)
{
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

	if (client_fd < 0) 
		throw Server::Error("accept failed.");
	
	this->connections.push();
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) < 0)
		throw Server::Error("Epoll_ctl failed");

}

void Server::listen(void)
{
	struct epoll_event event;
	int num_connections = 0;
	int epoll_fd = 0; 

	epoll_fd = epoll_create1(0);

	if (bind(this->fd, (struct sockaddr *)&this->server_addr, sizeof(server_addr)) < 0)
		throw Server::Error("Bind failed.");
	if (::listen(this->fd, this->max_events) < 0)
		throw Server::Error("Listen failed.");

	if (epoll_fd < 0)
		throw Server::Error("Epoll_create1 failed.");

	event.events = EPOLLIN;
	event.data.fd = this->fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, this->fd, &event))
		throw Server::Error("epoll_ctl failed.");
	while (true)
	{
		num_connections = epoll_wait(epoll_fd, events, this->max_events, -1);
		if (num_connections < 0)
			throw Server::Error("Epoll_wait failed.");
		for (size_t i = 0; i < num_connections; i++)
		{
			if (this->connections[i].data.fd == this->fd)
				accept_connection(epoll_fd);
			else
			{
				// handle_client request
			}
		}
	}

}


/*Exception class*/
Server::Error::Error(const char *_msg) : msg(_msg) {}

const char * Server::Error::what() const throw()
{
    return (msg);
}
	