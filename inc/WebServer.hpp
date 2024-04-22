/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 15:56:54 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/22 00:42:23 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <ServerContext.hpp>

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

#include <ConnStream.hpp>

typedef struct s_event_data
{
	int fd;
	char type;
	s_event_data(int _fd, char _type) : fd(_fd), type(_type) {}
} t_event_data;

enum SOCKET_TYPE
{
	SERVER = 's',
	CLIENT = 'c'
};

typedef struct epoll_event t_event;

class WebServer
{
	private:
		int max_events;
		int epoll_fd;
		int is_running; // We are going to set it to false with a signal.
		std::map<int, ServerContext *> servers;
	
		std::vector<t_event> events; // epoll_events

		// Each indivudal Connection
		std::map<int, ConnStream *> streams;
	public:
		WebServer();
		~WebServer();

		void listen(void);
		void init_servers(void);
		void accept_connection(int, int);
		void read_request(int, int, t_event);
		void send_response(int, int, t_event);
		void close_conn(int, int);

		class Error : public std::exception
		{
		private:
			const char *msg;
			virtual const char *what() const throw();

		public:
			Error(const char *_msg);
		};

		static int sfd_non_blocking(int);
		static int set_reuseaddr(int);
		static int epoll_add_fd(int, int, t_event);
		static int epoll_in_fd(int, int, t_event);
		static int epoll_out_fd(int, int, t_event);
		static int epoll_del_fd(int, int);
		static int bind(int, struct sockaddr_in *);
};
