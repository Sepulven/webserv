/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 13:22:29 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/09 14:02:35 by asepulve         ###   ########.fr       */
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

void Server::listen(void)
{
	
	if (bind(this->fd, (struct sockaddr *)&this->server_addr, sizeof(server_addr)) < 0)
		throw Server::Error("Bind failed.");
	if (::listen(this->fd, this->max_events) < 0)
		throw Server::Error("Listen failed.");
	
	// while (true)
	// {
		
	// }
}


/*Exception class*/
Server::Error::Error(const char *_msg) : msg(_msg) {}

const char * Server::Error::what() const throw()
{
    return (msg);
}
	