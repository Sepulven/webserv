/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 01:07:43 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/15 16:52:54 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <WebServer.hpp>

WebServer::WebServer()
{
	// We are going to create multiple server fds given the context;
	this->max_events = 0;
	this->init_servers();
	this->is_running = 1;
}

WebServer::~WebServer()
{
}

void	WebServer::init_servers(void)
{
	struct sockaddr_in server_addr;
	struct epoll_event event;
	std::vector<Server*> vec;

	vec.push_back(new Server("asepulve.com.br", "localhost", 8080));
	vec.push_back(new Server("ratavare.com.br", "localhost", 8088));
	vec.push_back(new Server("mvicent.com.br", "localhost", 8888));
	this->epoll_fd = epoll_create1(0);

	if (this->epoll_fd < 0)
		throw Error("Epoll_create1 failed.");
	for (size_t i = 0; i < vec.size(); i++)
	{
		memset(&server_addr, 0, sizeof(struct sockaddr_in));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = vec[i]->domain;
		server_addr.sin_port = htons(vec[i]->port);

		vec[i]->socket = socket(AF_INET, SOCK_STREAM, 0);
		if (vec[i]->socket < 0)
			throw Error("Socket failed.");
		if (sfd_non_blocking(vec[i]->socket) < 0)
			throw Error("Couldn't make the server socket non-blocking.");
		if (set_reuseaddr(vec[i]->socket) < 0)
			throw Error("Setsockopt failed");
		if (bind(vec[i]->socket, &server_addr) < 0)
			throw Error("Bind failed.");
		if (::listen(vec[i]->socket, vec[i]->max_events) < 0)
			throw Error("Listen failed.");

		memset(&event, 0, sizeof(struct epoll_event));
		event.events = EPOLLIN;
		event.data.ptr = new t_events(vec[i]->socket, SERVER);
		if (epoll_add_fd(this->epoll_fd, vec[i]->socket, event))
			throw Error("epoll_ctl failed.");
		this->max_events += vec[i]->max_events;
		std::cout << "[" << vec[i]->max_events << "] Listening on port: " << vec[i]->port << std::endl;
	}
	//We ensure that our vector won't change its memory area.
	this->events.reserve(this->max_events);
}


void WebServer::accept_connection(int epoll_fd, int fd)
{
	struct epoll_event event;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);

	if (client_fd < 0)
		throw Error("accept failed.");
	if (sfd_non_blocking(client_fd) < 0)
		throw Error("Couln't make socket fd non-blocking.");

	event.events = EPOLLIN | EPOLLET;
	event.data.ptr = new t_events(client_fd, CLIENT);

	if (WebServer::epoll_add_fd(epoll_fd, client_fd, event) < 0)
		throw Error("Epoll_ctl failed");
}

void WebServer::send_request(int epoll_fd, int fd, struct epoll_event event)
{
	Req req(fd, (this->requests)[fd]);
	req.process_request();
	std::cout << "end 0\n";
	this->requests.erase(fd);
	std::cout << "end 1\n";
	
	if (epoll_in_fd(epoll_fd, fd, event) < 0)
		throw Error("Epoll_ctl failed");

	std::cout << "end 2\n";
}

void WebServer::close_conn(int epoll_fd, int fd)
{
	if (epoll_del_fd(epoll_fd, fd) < 0)
		throw Error("Epoll_ctl failed");
	close(fd);
}

std::pair<std::string, std::string> split1(std::string str, char c)
{
    std::size_t found = str.find(c);
    std::string s1 = str.substr(0, found);
    std::string s2 = str.substr(found + 1);
    
    return std::make_pair(s1, s2);
}

void WebServer::read_request(int epoll_fd, int fd, struct epoll_event event)
{
	char buffer[1025];
	std::string &str = this->request;
	int bytes_read = read(fd, buffer, 1024);
	buffer[bytes_read] = '\0';

	if (bytes_read <= 0)
	{
		this->close_conn(epoll_fd, fd);
		return ;
	}

	std::string aux;
	aux = buffer;
	std::cout << "this->request: " << aux << "." << std::endl;

	std::string method = split1(aux, ' ').first;
	std::string end;
	int nb;

	if (method == "GET" || method == "DELETE")
	{
		end = "\r\n\r\n";
		nb = 4;
	}
	else if (method == "POST")
	{
		int i = aux.find("boundary=");
		if (i == -1)
			end = "\r\n\r\n";
		else
		{
			i = i + 9;
			int f = aux.find("\r\n", i);

			end = aux.substr(i, f - i) + "--\r\n";
			nb = end.size();
		}
	}
	else
		throw std::invalid_argument("");


	while (!aux.find(end))
    {
        buffer[bytes_read] = '\0';
        aux =+ buffer;
        bytes_read = read(fd, buffer, sizeof(buffer));
    }

	this->requests.insert(make_pair(fd, aux));

	buffer[bytes_read] = 0;

	str = aux;
	
	if (!strcmp(&(str.c_str()[str.size() - nb]), end.c_str()))
	{
		std::cout << "check fim\n";
		if (epoll_out_fd(epoll_fd, fd, event))
			throw Error("Epoll_ctl failed");
	}
	else
	{
		std::cout << "file not supported\n";
		throw Error("");
	}
}

void WebServer::listen(void)
{
	int num_events = 0;
	int epoll_fd = this->epoll_fd; 
	struct epoll_event *conn;

	while (WebServer::is_running)
	{
		num_events = epoll_wait(epoll_fd, this->events.data(), this->max_events, -1);
		if (num_events < 0)
			throw Error("Epoll_wait failed.");
		for (int i = 0; i < num_events; i++)
		{
			conn = &this->events[i];
			t_events* event_data = (t_events*)conn->data.ptr;
			if ((event_data->type == SERVER) && (conn->events & EPOLLIN))
			{
				std::cout << "Accept_connection:" << std::endl;
				accept_connection(epoll_fd, event_data->fd);
			}
			else if (conn->events & EPOLLIN)
			{
				std::cout << "read request:" << std::endl;
				read_request(epoll_fd, event_data->fd, *conn);
			}
			else if (conn->events & EPOLLOUT)
			{
				std::cout << "epoll out event" << std::endl;
				send_request(epoll_fd, event_data->fd, *conn);
			}
			// I need to forcefully test this scnerario for better error handling.
			else if (conn->events & EPOLLERR || conn->events & EPOLLHUP)
			{
				std::cout << "Conn. closed in " << event_data->fd << std::endl;
				close_conn(epoll_fd, event_data->fd);
			}
		}
		this->events.clear();
	}
	std::cout << std::endl << std::endl << "SERVER DIED" << std::endl;
}

/*Exception class*/
WebServer::Error::Error(const char *_msg) : msg(_msg) {}

const char * WebServer::Error::what() const throw()
{
    return (msg);
}
