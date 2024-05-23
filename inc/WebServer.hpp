#pragma once

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

/* Classes */
#include <ConnStream.hpp>
#include <ServerError.hpp>
#include <ServerContext.hpp>


/* Utils */
#include <__webserv_utils.hpp>

static volatile sig_atomic_t is_running = 1;

using namespace ServerUtils;

class WebServer
{

	private:
		int max_events;
		int epoll_fd;
		std::map<int, ServerContext *> servers;
	
		std::vector<t_event> events; // epoll_events

		// * Connections
		std::map<int, ConnStream *> streams;
	public:
		WebServer(std::list<t_server> serverNodes);
		~WebServer();

		void listen(void);
		void init_servers(std::vector<ServerContext *>&);
		void accept_connection(int, int);
		void read_request(int, int, t_event);
		void send_response(int, int, t_event);
		void close_conn(int, int);
		void time_out(int);

		static void	sig_handler(int sig); // * Handles Ctrl + c
};
