#include <WebServer.hpp>



WebServer::WebServer()
{
	this->max_events = 0;
	this->init_servers();
	signal(SIGINT, &WebServer::sig_handler);
}

WebServer::~WebServer()
{
	std::cout << "\r  ";
	std::cout << "\033[31m" << std::endl;
	std::cout << "                                                     " << std::endl;
	std::cout << " ▒█████    █████▒ █████▒██▓     ██▓ ███▄    █ ▓█████ " << std::endl;
	std::cout << "▒██▒  ██▒▓██   ▒▓██   ▒▓██▒    ▓██▒ ██ ▀█   █ ▓█   ▀ " << std::endl;
	std::cout << "▒██░  ██▒▒████ ░▒████ ░▒██░    ▒██▒▓██  ▀█ ██▒▒███   " << std::endl;
	std::cout << "▒██   ██░░▓█▒  ░░▓█▒  ░▒██░    ░██░▓██▒  ▐▌██▒▒▓█  ▄ " << std::endl;
	std::cout << "░ ████▓▒░░▒█░   ░▒█░   ░██████▒░██░▒██░   ▓██░░▒████▒" << std::endl;
	std::cout << "░ ▒░▒░▒░  ▒ ░    ▒ ░   ░ ▒░▓  ░░▓  ░ ▒░   ▒ ▒ ░░ ▒░ ░" << std::endl;
	std::cout << "  ░ ▒ ▒░  ░      ░     ░ ░ ▒  ░ ▒ ░░ ░░   ░ ▒░ ░ ░  ░" << std::endl;
	std::cout << "░ ░ ░ ▒   ░ ░    ░ ░     ░ ░    ▒ ░   ░   ░ ░    ░   " << std::endl;
	std::cout << "    ░ ░                    ░  ░ ░           ░    ░  ░" << std::endl << "\033[0m";
	for (std::vector<Server*>::iterator it = servers.begin(); it != servers.end(); it++)
		delete *it;
	for (std::map<int, t_events*>::iterator it = conn.begin(); it != conn.end(); it++)
		close_conn(this->epoll_fd, it->second->fd);
		// std::cout << it->second->fd << std::endl;
}

void WebServer::init_servers(void)
{
	struct sockaddr_in server_addr;
	struct epoll_event event;
	int server_fd;
	std::vector<ServerContext *> vec;

	vec.push_back(new ServerContext("asepulve.com.br", "localhost", 8080));
	vec.push_back(new ServerContext("ratavare.com.br", "localhost", 8088));
	vec.push_back(new ServerContext("mvicent.com.br", "localhost", 8888));

	this->epoll_fd = epoll_create1(0);
	if (this->epoll_fd < 0)
		throw Error("Epoll_create1 failed.");
	for (size_t i = 0; i < vec.size(); i++)
	{
		memset(&server_addr, 0, sizeof(struct sockaddr_in));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = vec[i]->domain;
		server_addr.sin_port = htons(vec[i]->port);

		server_fd = socket(AF_INET, SOCK_STREAM, 0);
		vec[i]->socket = server_fd;
		if (server_fd < 0)
			throw Error("Socket failed.");
		if (sfd_non_blocking(server_fd) < 0)
			throw Error("Couldn't make the server socket non-blocking.");
		if (set_reuseaddr(server_fd) < 0)
			throw Error("Setsockopt failed");
		if (bind(server_fd, &server_addr) < 0)
			throw Error("Bind failed.");
		if (::listen(server_fd, vec[i]->max_events) < 0)
			throw Error("Listen failed.");

		memset(&event, 0, sizeof(struct epoll_event));
		event.events = EPOLLIN;
		event.data.ptr = new t_event_data(server_fd, SERVER);
		if (epoll_add_fd(this->epoll_fd, server_fd, event))
			throw Error("epoll_ctl failed.");
		this->max_events += vec[i]->max_events;
		this->servers[server_fd] = vec[i];
		std::cout << "[" << vec[i]->max_events << "] Listening on port: " << vec[i]->port << std::endl;
	}
	this->events.reserve(this->max_events);
	this->servers = vec;
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
	event.data.ptr = new t_event_data(client_fd, CLIENT);

	if (WebServer::epoll_add_fd(epoll_fd, client_fd, event) < 0)
		throw Error("Epoll_ctl failed");
	this->streams[client_fd] = new ConnStream(client_fd, servers[fd]);
}

void WebServer::send_response(int epoll_fd, int fd, t_event event)
{
	int status = this->streams[fd]->res->send();

	if ((status > 0) && epoll_in_fd(epoll_fd, fd, event) < 0)
		throw Error("Epoll_ctl failed");
	if (status == -1)
		close_conn(epoll_fd, fd);
}

void WebServer::read_request(int epoll_fd, int fd, t_event event)
{
	int status = this->streams[fd]->req->read(fd);

	if ((status == 1) && epoll_out_fd(epoll_fd, fd, event))
		throw Error("Epoll_ctl failed");
	if (status == -1)
		this->close_conn(epoll_fd, fd);
}

void WebServer::listen(void)
{
	int num_events = 0;
	int epoll_fd = this->epoll_fd;
	t_event *conn;
	t_event_data *event_data;

	while (is_running)
	{
		num_events = epoll_wait(epoll_fd, this->events.data(), this->max_events, -1);
		if (num_events < 0 || !is_running)
			break;
		for (int i = 0; i < num_events; i++)
		{
			conn = &this->events[i];
			event_data = static_cast<t_event_data *>(conn->data.ptr);
			if ((event_data->type == SERVER) && (conn->events & EPOLLIN))
				accept_connection(epoll_fd, event_data->fd);
			else if (conn->events & EPOLLIN)
				read_request(epoll_fd, event_data->fd, *conn);
			else if (conn->events & EPOLLOUT)
				send_response(epoll_fd, event_data->fd, *conn);
			else if (conn->events & EPOLLERR || conn->events & EPOLLHUP)
				close_conn(epoll_fd, event_data->fd);
		}
		this->events.clear();
	}
	std::cout << std::endl
			  << std::endl
			  << "SERVER DIED" << std::endl;
}

/*Exception class*/
WebServer::Error::Error(const char *_msg) : msg(_msg) {}

const char *WebServer::Error::what() const throw()
{
	return (msg);
}
