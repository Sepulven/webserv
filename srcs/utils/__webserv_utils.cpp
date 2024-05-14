#include <__webserv_utils.hpp>

int ServerUtils::sfd_non_blocking(int sfd)
{
	return fcntl(sfd, F_SETFL, O_NONBLOCK | FD_CLOEXEC);
}

int ServerUtils::set_reuseaddr(int sfd)
{
	int enable = 1;

	return (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)));
}

#include <iostream>

int ServerUtils::bind(int sfd, struct sockaddr_in *server_addr)
{
	if (!server_addr)
		return (-1);

	return (::bind(sfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr_in)));
}

int ServerUtils::epoll_add_fd(int epoll_fd, int fd, t_event event)
{
	return (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event));
}

int ServerUtils::epoll_in_fd(int epoll_fd, int fd, t_event event)
{
	event.events = EPOLLIN | EPOLLET;
	return (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event));
}

int ServerUtils::epoll_out_fd(int epoll_fd, int fd, t_event event)
{
	event.events = EPOLLOUT | EPOLLET;
	return (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event));
}

int ServerUtils::epoll_del_fd(int epoll_fd, int fd)
{
	return (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL));
}
