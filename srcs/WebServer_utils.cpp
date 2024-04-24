#include <WebServer.hpp>

int WebServer::sfd_non_blocking(int sfd)
{
	return fcntl(sfd, F_SETFL, O_NONBLOCK | FD_CLOEXEC);
}

int WebServer::set_reuseaddr(int sfd)
{
	int enable = 1;

	return (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)));
}

int WebServer::bind(int sfd, struct sockaddr_in *server_addr)
{
	if (!server_addr)
		return (-1);
	return (::bind(sfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr_in)));
}

int WebServer::epoll_add_fd(int epoll_fd, int fd, t_event event)
{
	return (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event));
}

int WebServer::epoll_in_fd(int epoll_fd, int fd, t_event event)
{
	// event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
	event.events = EPOLLIN | EPOLLET;
	return (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event));
}

int WebServer::epoll_out_fd(int epoll_fd, int fd, t_event event)
{
	// event.events = EPOLLOUT | EPOLLERR | EPOLLHUP;
	event.events = EPOLLOUT | EPOLLET;
	return (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event));
}

int WebServer::epoll_del_fd(int epoll_fd, int fd)
{
	return (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL));
}

/*
	* Close a conn, and clean up the resources.
*/
void WebServer::close_conn(int epoll_fd, int fd)
{
	if (epoll_del_fd(epoll_fd, fd) < 0)
		throw Error("Epoll_ctl failed");
	close(fd);
	delete this->streams[fd];
}


/***********************************/

std::pair<std::string, std::string> split1(std::string str, char c)
{
    std::size_t found = str.find(c);
    std::string s1 = str.substr(0, found);
    std::string s2 = str.substr(found + 1);

    return std::make_pair(s1, s2);
}
