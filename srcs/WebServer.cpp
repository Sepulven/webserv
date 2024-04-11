/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebWebserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 15:56:38 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/11 16:15:20 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Webserver.hpp>

Webserver::Webserver() : domain(AF_INET), port(8080)
{
	this->max_events = 200;
	

	this->fd = socket(this->domain, SOCK_STREAM, 0);
	if (this->fd < 0)
		throw Webserver::Error("Socket failed.");
	if (Webserver::sfd_non_blocking(this->fd))
		throw Webserver::Error("Couldn't make the Webserver socket non-blocking.");
	memset(&this->Webserver_addr, 0, sizeof(struct sockaddr_in));
	this->Webserver_addr.sin_family = this->domain;
	this->Webserver_addr.sin_addr.s_addr = INADDR_ANY;
	this->Webserver_addr.sin_port = htons(this->port);

	//We ensure that our vector won't change its area.
	this->connections.reserve(this->max_events);

	int enable = 1;
	if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw Webserver::Error("Setsockopt failed");

	epoll_fd = epoll_create1(0);


	if (bind(this->fd, (struct sockaddr *)&this->Webserver_addr, sizeof(Webserver_addr)) < 0)
		throw Webserver::Error("Bind failed.");
	if (::listen(this->fd, this->max_events) < 0)
		throw Webserver::Error("Listen failed.");

	if (epoll_fd < 0)
		throw Webserver::Error("Epoll_create1 failed.");

	if (Webserver::sfd_non_blocking(epoll_fd) < 0)
		throw Webserver::Error("Couln't make epoll fd non-blocking.");

	event.events = EPOLLIN;
	event.data.fd = this->fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, this->fd, &event))
		throw Webserver::Error("epoll_ctl failed.");
}

Webserver::~Webserver()
{
	close(this->fd);
}

void Webserver::accept_connection(int epoll_fd)
{
	struct epoll_event event;

	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(this->fd, (struct sockaddr *)&client_addr, &client_addr_len);

	if (client_fd < 0) 
		throw Webserver::Error("accept failed.");

	if (sfd_non_blocking(client_fd) < 0)
		throw Webserver::Error("Couln't make socket fd non-blocking.");
	event.events = EPOLLIN;
	event.data.fd = client_fd;

	this->connections.push_back(event);
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) < 0)
		throw Webserver::Error("Epoll_ctl failed");
}

void Webserver::read_request(int epoll_fd, struct epoll_event conn, int i)
{
	char buffer[1024];
	std::string &str = this->request;
	std::string suffix = "\r\n\r\n";
	int bytes_read = read(conn.data.fd, buffer, 1024);

	buffer[bytes_read] = 0;
	std::cout << buffer << std::endl;
	std::cout << "bytes_read:" << bytes_read << std::endl;

	str += buffer;
	std::cout << str << std::endl;

	if (!strcmp(&(str.c_str()[str.size() - 4]), "\r\n\r\n"))
	{
		char buffer1[1024] = "HTTP/1.1 200 OK\n"
							"Date: Mon, 27 Jul 2009 12:28:53 GMT\n"
							"Webserver: Apache/2.2.14 (Win32)\n"
							"Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n"
							"Content-Length: 88\n"
							"Content-Type: text/html\n"
							"Connection: Closed\n"
							"\n"
							"<html>\n"
							"<body>\n"
							"<img src='./42.jpeg' alt="">"
							"<h1>Hello, World!</h1>\n"
							"</body>\n"
							"</html>\n"
							"\r\n";
		write(conn.data.fd, buffer1, sizeof(buffer1));

		// Remove it from the list of watched elements
		if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn.data.fd, NULL) < 0)
			throw Webserver::Error("Epoll_ctl failed here");

		// Close the connection and finishes the request
		close(conn.data.fd);

		// We must erase the element without changing the order of the elements.
		// So epoll could work
		this->connections.erase(this->connections.begin() + i);
	}
}

void Webserver::listen(void)
{
		struct epoll_event event;
		int num_connections = 0;
		int epoll_fd = this->epoll_fd; 
		struct epoll_event current_connection;

		while (this->fd != -1)
		{
			num_connections = epoll_wait(epoll_fd, this->connections.data(), this->max_events, -1);
			if (num_connections < 0)
				throw Webserver::Error("Epoll_wait failed.");
			for (int i = 0; i < num_connections; i++)
			{
				current_connection = this->connections[i];
				if ((current_connection.data.fd == this->fd) && (current_connection.events & EPOLLIN))
					accept_connection(epoll_fd);
				else if (this->connections[i].events & EPOLLIN)
					read_request(epoll_fd, current_connection, i);
			}
		}
}

/*Exception class*/
Webserver::Error::Error(const char *_msg) : msg(_msg) {}

const char * Webserver::Error::what() const throw()
{
    return (msg);
}
