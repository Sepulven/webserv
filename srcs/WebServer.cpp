/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 01:07:43 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/15 11:00:35 by asepulve         ###   ########.fr       */
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
	vec.push_back(new Server("asepulve.com.br", "localhost", 8080));
	vec.push_back(new Server("ratavare.com.br", "localhost", 8088));
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
		std::cout << vec[i]->port << std::endl;
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
		event.data.ptr = new t_events(vec[i]->socket, strdup("server"));
		if (WebServer::epoll_add_fd(this->epoll_fd, vec[i]->socket, event))
			throw WebServer::Error("epoll_ctl failed.");
	}

	this->max_events = 0;
	for (int i = 0; i < 3; i++)
		this->max_events += vec[i]->max_events;

	//We ensure that our vector won't change its memory area.
	this->connections.reserve(this->max_events);
}


void WebServer::accept_connection(int epoll_fd, int fd)
{
	struct epoll_event event;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);

	std::cout << client_fd << std::endl;
	if (client_fd < 0)
		throw WebServer::Error("accept failed.");
	if (sfd_non_blocking(client_fd) < 0)
		throw WebServer::Error("Couln't make socket fd non-blocking.");
	event.events = EPOLLIN;
	event.data.ptr = new t_events(client_fd, strdup("client"));

	t_events* ptr = (t_events*)event.data.ptr;
	std::cout << "fd that has been accepted" << ptr->fd << std::endl;
	if (WebServer::epoll_add_fd(epoll_fd, client_fd, event) < 0)
		throw WebServer::Error("Epoll_ctl failed");
}

void WebServer::read_request(int epoll_fd, int fd)
{
	char buffer[1024];
	std::string &str = this->request;
	std::string suffix = "\r\n\r\n";
	int bytes_read = read(fd, buffer, 1024);

	if (bytes_read < 0)
		return ;

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
		write(fd, buffer1, sizeof(buffer1));
		
		if (WebServer::epoll_del_fd(epoll_fd, fd))
		// Close the connection and finishes the request
		close(fd);
	}
}

void WebServer::listen(void)
{
		int num_connections = 0;
		int epoll_fd = this->epoll_fd; 
		struct epoll_event *conn;

		while (WebServer::is_running)
		{
			num_connections = epoll_wait(epoll_fd, this->connections.data(), this->max_events, -1);
			if (num_connections < 0)
				throw WebServer::Error("Epoll_wait failed.");
	
			for (int i = 0; i < num_connections; i++)
			{
				conn = &this->connections[i];
				t_events* event_data = (t_events*)conn->data.ptr;
				if ((!strcmp(event_data->type, "server")) && (conn->events & EPOLLIN))
				{
					std::cout << "Accept_connection:" << std::endl;
					accept_connection(epoll_fd, event_data->fd);
				}
				else if (this->connections[i].events & EPOLLIN)
				{
					// std::cout << "Read request: " << std::endl;
					read_request(epoll_fd, event_data->fd);
				}
			}
			this->connections.clear();
		}
}

/*Exception class*/
WebServer::Error::Error(const char *_msg) : msg(_msg) {}

const char * WebServer::Error::what() const throw()
{
    return (msg);
}
