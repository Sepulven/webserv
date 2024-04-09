/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 12:31:02 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/09 12:41:46 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class Server
{
public:
	Server();
	Server(Server &&) = default;
	Server(const Server &) = default;
	Server &operator=(Server &&) = default;
	Server &operator=(const Server &) = default;
	~Server();
private:
	struct sockaddr_in  server_addr;
	int fd;
	/*CGI - may be a new object */
	unsigned short int PORT;
	struct epoll_event *connections;
	int max_events;
};

Server::Server()
{
}

Server::~Server()
{
}