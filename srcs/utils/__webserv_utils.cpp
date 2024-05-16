#include <__webserv_utils.hpp>

/*
 * Makes specified fd non blocking using fcntl;
*/
int ServerUtils::sfd_non_blocking(int sfd)
{
	return fcntl(sfd, F_SETFL, O_NONBLOCK | FD_CLOEXEC);
}

/*
 * Set reuseaddr, avoid system's error;
*/
int ServerUtils::set_reuseaddr(int sfd)
{
	int enable = 1;

	return (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)));
}

/*
 * Binds a fd to a port;
*/
int ServerUtils::bind(int sfd, struct sockaddr_in *server_addr)
{
	if (!server_addr)
		return (-1);
	return (::bind(sfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr_in)));
}


/*
 * Adds the fd to the data structure of monitored fd in epoll;
*/
int ServerUtils::epoll_add_fd(int epoll_fd, int fd, t_event event)
{
	return (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event));
}

/*
 * Removes the fd from the data structure of monitored fd in epoll;
*/
int ServerUtils::epoll_del_fd(int epoll_fd, int fd)
{
	return (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL));
}

/*
 * Changes the event to EPOLLIN and EPOLLET;
 	* -> EPOLLET(edgy triggered) , the secret of performance;
*/
int ServerUtils::epoll_in_fd(int epoll_fd, int fd, t_event event)
{
	event.events = EPOLLIN | EPOLLET;
	return (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event));
}

/*
 * Changes the event to EPOLLOUT and EPOLLET;
 	* -> EPOLLET(edgy triggered) , the secret of performance;
*/
int ServerUtils::epoll_out_fd(int epoll_fd, int fd, t_event event)
{
	event.events = EPOLLOUT | EPOLLET;
	return (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event));
}

