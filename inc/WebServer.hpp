/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 15:56:54 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/12 16:51:58 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <Server.hpp>

/* C headers*/
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <string.h>

/* C++ headers*/
#include <iostream>
#include <exception>
#include <vector>
#include <algorithm>

// #include <sys/stat.h>
// #include <sys/types.h>

class WebServer
{
private:
	int max_events;
	int epoll_fd;
	int is_running; // We are going to set is to false with a signal.
	std::vector<Server *> servers; 
	std::vector<struct epoll_event> connections;
public:
	WebServer();
	~WebServer();

	/*test purpose*/
	std::string request;

	void	listen(void);
	void	init_servers(void);
	void	accept_connection(int, struct epoll_event );
	void	read_request(int, struct epoll_event, int);

	class Error : public std::exception
	{
		private:
			const char *msg;
			virtual const char * what() const throw();
		public:
			Error(const char *_msg);
	};
	static int	sfd_non_blocking(int);
	static int	set_reuseaddr(int);
	static int	epoll_add_fd(int, int, struct epoll_event *);
	static int	bind(int, struct sockaddr_in *);
};
