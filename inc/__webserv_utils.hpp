#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace ServerUtils
{
	typedef struct epoll_event t_event;

	int sfd_non_blocking(int);
	int set_reuseaddr(int);
	int epoll_add_fd(int, int, t_event);
	int epoll_in_fd(int, int, t_event);
	int epoll_out_fd(int, int, t_event);
	int epoll_del_fd(int, int);
	int bind(int, struct sockaddr_in *);
};
