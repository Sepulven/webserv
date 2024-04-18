/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 15:56:54 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/18 12:41:31 by asepulve         ###   ########.fr       */
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
#include <stdio.h>

#include <string.h>

/* C++ headers*/
#include <iostream>
#include <exception>
#include <vector>
#include <algorithm>

// #include <sys/stat.h>
// #include <sys/types.h>

typedef struct s_events {
	int fd;
	char type;
	s_events(int _fd, char _type) : fd(_fd), type(_type) {}
} t_events;

enum SOCKET_TYPE
{
	SERVER = 's',
	CLIENT = 'c'
};

typedef struct epoll_event t_event;

static volatile sig_atomic_t is_running = 1;

class WebServer
{
private:
	int							max_events;
	int							epoll_fd;
	
	std::vector<Server *>		servers; 
	std::vector<t_event>		events;

	std::string					line_w;
	std::map<int, std::string>	requests;
	std::map<int, t_events*>		conn;

public:
	WebServer();
	~WebServer();

	/*test purpose*/
	std::string request;

	void	listen(void);
	void	init_servers(void);
	void	accept_connection(int, int);
	void	read_request(int, int, t_event);
	void	send_response(int, int, t_event);
	void	close_conn(int, int);
	static void	sig_handler(int sig);

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
	static int	epoll_add_fd(int, int, t_event);
	static int	epoll_in_fd(int, int, t_event);
	static int	epoll_out_fd(int, int, t_event);
	static int	epoll_del_fd(int, int);
	static int	bind(int, struct sockaddr_in *);
};
