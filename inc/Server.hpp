/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 12:31:02 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/09 17:19:23 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/* C headers*/
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

/* C++ headers*/
#include <iostream>
#include <exception>
#include <vector>

// #include <sys/stat.h>
// #include <sys/types.h>

class Server
{
	private:
		struct sockaddr_in  server_addr;
		int domain;
		int fd;
	
		/*CGI - may be a new object */
		int port;
		std::vector<struct epoll_event> connections;
		int max_events;
	public:
		void	listen(void);
		void	accept_connection(int);

		Server();
		~Server();

		class Error : public std::exception
		{
			private:
				const char *msg;
				virtual const char * what() const throw();
			public:
				Error(const char *_msg);
		};
};