#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>



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
