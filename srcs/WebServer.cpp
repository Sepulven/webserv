/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 01:07:43 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/12 01:44:47 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <WebServer.hpp>

WebServer::WebServer()
{
	// We are going to create multiple server fds given the context;
	this->init_servers();
}

WebServer::~WebServer()
{
}

void	WebServer::init_servers(void)
{
	std::vector<Server*> vec;
	vec.push_back(new Server("asepulve.com.br", "localhost", 80));
	vec.push_back(new Server("ratavare.com.br", "localhost", 8080));
	vec.push_back(new Server("mvicent.com.br", "localhost", 8888));
	struct sockaddr_in server_addr;
	struct epoll_event event;

	this->epoll_fd = epoll_create1(0);
	
	if (this->epoll_fd < 0)
		throw WebServer::Error("Epoll_create1 failed.");
	
	for (size_t i = 0; i < vec.size(); i++)
	{
		memset(&server_addr, 0, sizeof(struct sockaddr_in));

		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = vec[i]->domain;
		server_addr.sin_port = htons(vec[i]->port);
	

		vec[i]->socket = socket(AF_INET, SOCK_STREAM, 0);
		if (vec[i]->socket < 0)
			throw WebServer::Error("Socket failed.");
		if (WebServer::sfd_non_blocking(vec[i]->socket) < 0)
			throw WebServer::Error("Couldn't make the server socket non-blocking.");
		if (WebServer::set_reuseaddr(vec[i]->socket) < 0)
			throw WebServer::Error("Setsockopt failed");
		if (WebServer::bind(vec[i]->socket, &server_addr) < 0)
			throw WebServer::Error("Bind failed.");
		if (::listen(vec[i]->socket, vec[i]->max_events) < 0)
			throw WebServer::Error("Listen failed.");

		memset(&event, 0, sizeof(struct epoll_event));
		event.events = EPOLLIN;
		event.data.fd = vec[i]->socket;
		event.data.ptr = NULL; // We can add a lot of extra information in here whenever an event triggers;
		if (WebServer::epoll_add_fd(this->epoll_fd, vec[i]->socket, &event))
			throw WebServer::Error("epoll_ctl failed.");
	}

	this->max_events = 0;
	for (int i = 0; i < 3; i++)
		this->max_events += vec[i]->max_events;

	//We ensure that our vector won't change its memory area.
	this->connections.reserve(this->max_events);
}


void WebServer::accept_connection(int epoll_fd)
{
	struct epoll_event event;

	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(this->fd, (struct sockaddr *)&client_addr, &client_addr_len);

	if (client_fd < 0) 
		throw WebServer::Error("accept failed.");

	if (sfd_non_blocking(client_fd) < 0)
		throw WebServer::Error("Couln't make socket fd non-blocking.");
	event.events = EPOLLIN;
	event.data.fd = client_fd;

	this->connections.push_back(event);
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) < 0)
		throw WebServer::Error("Epoll_ctl failed");
}

void WebServer::read_request(int epoll_fd, struct epoll_event conn, int i)
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
			throw WebServer::Error("Epoll_ctl failed here");

		// Close the connection and finishes the request
		close(conn.data.fd);

		// We must erase the element without changing the order of the elements.
		// So epoll could work
		this->connections.erase(this->connections.begin() + i);
	}
}

void WebServer::listen(void)
{
		int num_connections = 0;
		int epoll_fd = this->epoll_fd; 
		struct epoll_event current_connection;

		while (WebServer::is_running)
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
WebServer::Error::Error(const char *_msg) : msg(_msg) {}

const char * WebServer::Error::what() const throw()
{
    return (msg);
}
