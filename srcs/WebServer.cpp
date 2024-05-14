#include <WebServer.hpp>


/*
 * Inits the webserver;
 * Receives serverNodes from parser t start everything;
*/
WebServer::WebServer(std::list<t_server> serverNodes)
{
	std::list<t_server>::iterator it = serverNodes.begin();
	std::vector<ServerContext *> vec;

	this->max_events = 0;

	signal(SIGINT, &WebServer::sig_handler);
	for (; it != serverNodes.end(); it++)
		vec.push_back(new ServerContext(*it));
	this->init_servers(vec);

	// * Logs the online logo;
	std::cout << "\r" << "\033[32m" << std::endl
		<<  "                                                  " << std::endl
		<<  " ▒█████   ███▄    █  ██▓     ██▓ ███▄    █ ▓█████ " << std::endl
		<<  "▒██▒  ██▒ ██ ▀█   █ ▓██▒    ▓██▒ ██ ▀█   █ ▓█   ▀ " << std::endl
		<<  "▒██░  ██▒▓██  ▀█ ██▒▒██░    ▒██▒▓██  ▀█ ██▒▒███   " << std::endl
		<<  "▒██   ██░▓██▒  ▐▌██▒▒██░    ░██░▓██▒  ▐▌██▒▒▓█  ▄ " << std::endl
		<<  "░ ████▓▒░▒██░   ▓██░░██████▒░██░▒██░   ▓██░░▒████▒" << std::endl
		<<  "░ ▒░▒░▒░ ░ ▒░   ▒ ▒ ░ ▒░▓  ░░▓  ░ ▒░   ▒ ▒ ░░ ▒░ ░" << std::endl
		<<  "  ░ ▒ ▒░ ░ ░░   ░ ▒░░ ░ ▒  ░ ▒ ░░ ░░   ░ ▒░ ░ ░  ░" << std::endl
		<<  "░ ░ ░ ▒     ░   ░ ░   ░ ░    ▒ ░   ░   ░ ░    ░   " << std::endl
		<<  "    ░ ░           ░     ░  ░ ░           ░    ░  ░" << std::endl
		<<  "                                                  " << std::endl << "\033[0m";
}

WebServer::~WebServer()
{
	
	std::cout << "\r" << "\033[31m" << std::endl
		 << "                                                     " << std::endl
		 << " ▒█████    █████▒ █████▒██▓     ██▓ ███▄    █ ▓█████ " << std::endl
		 << "▒██▒  ██▒▓██   ▒▓██   ▒▓██▒    ▓██▒ ██ ▀█   █ ▓█   ▀ " << std::endl
		 << "▒██░  ██▒▒████ ░▒████ ░▒██░    ▒██▒▓██  ▀█ ██▒▒███   " << std::endl
		 << "▒██   ██░░▓█▒  ░░▓█▒  ░▒██░    ░██░▓██▒  ▐▌██▒▒▓█  ▄ " << std::endl
		 << "░ ████▓▒░░▒█░   ░▒█░   ░██████▒░██░▒██░   ▓██░░▒████▒" << std::endl
		 << "░ ▒░▒░▒░  ▒ ░    ▒ ░   ░ ▒░▓  ░░▓  ░ ▒░   ▒ ▒ ░░ ▒░ ░" << std::endl
		 << "  ░ ▒ ▒░  ░      ░     ░ ░ ▒  ░ ▒ ░░ ░░   ░ ▒░ ░ ░  ░" << std::endl
		 << "░ ░ ░ ▒   ░ ░    ░ ░     ░ ░    ▒ ░   ░   ░ ░    ░   " << std::endl
		 << "    ░ ░                    ░  ░ ░           ░    ░  ░" << std::endl << "\033[0m";
	for (std::map<int, ConnStream*>::iterator it = streams.begin(); it != streams.end(); it++)
		close_conn(this->epoll_fd, it->second->fd);
	for (std::map<int, ServerContext*>::iterator it = servers.begin(); it != servers.end(); it++)
		delete it->second;
}

/*
 * Starts the servers to listen to a specific port;
*/
void WebServer::init_servers(std::vector<ServerContext *>& vec)
{
	struct sockaddr_in server_addr;
	struct epoll_event event;
	int server_fd;

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
		std::cout << "[" << vec[i]->max_events << "] Listening on port: " << vec[i]->port << std::endl;
	}
	this->events.reserve(this->max_events);
}

/*
 * Creates a new connection fd and push it to the pool;
 * In case of it couln't accept the connection if throws an error;
*/
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

	if (epoll_add_fd(epoll_fd, client_fd, event) < 0)
		throw Error("Epoll_ctl failed");
	this->streams[client_fd] = new ConnStream(client_fd, servers[fd]);
}

/*
 * Process the request and send the response for a specific connection;
 * In case of error closes the connections;
*/
void WebServer::send_response(int epoll_fd, int fd, t_event event)
{
	int status = this->streams[fd]->res->send();
	this->streams[fd]->set_time(); // * Update last action;
	if ((status > 0) && epoll_in_fd(epoll_fd, fd, event) < 0)
		throw Error("Epoll_ctl failed");
	this->streams[fd]->clean_conn();
	if (status == -1)
		close_conn(epoll_fd, fd);
}

/*
 * Process the request until it reaches the end condition, 
 * either content-length or the end of the header;
 * In case of error closes the connection;
*/
void WebServer::read_request(int epoll_fd, int fd, t_event event)
{
	int status = this->streams[fd]->req->read(fd);

	this->streams[fd]->set_time();
	if ((status == 1) && epoll_out_fd(epoll_fd, fd, event))
		throw Error("Epoll_ctl failed");
	if (status == -1)
		this->close_conn(epoll_fd, fd);
}

/*
* Kills CGI if it hasn't finished yet, checks CGI time out and CGI status;
* Kills connections after the close_conn_time has exceeded;
*/
void WebServer::time_out(int epoll_fd)
{
	std::map<int, ConnStream *>::iterator it = this->streams.begin(), ite = this->streams.end();
	std::vector<int> keys_to_delete;
	ConnStream * current_conn;
	long long	current_time;
	struct timeval		t;

	gettimeofday(&t, NULL);
	current_time = (t.tv_sec * 1000) + (t.tv_usec / 1000);
	std::cout << "Number of conn:" << this->streams.size() << std::endl;
	for (; it != ite; it++)
	{
		current_conn = it->second;
		int wpid;
		wpid = waitpid(current_conn->cgi_pid, NULL, WNOHANG);
		std::cout << wpid << std::endl;
		if (wpid == -1)
			current_conn->cgi_pid = -1;
		if (wpid > 0) // * pid of the child process has changed
			current_conn->cgi_pid = -1;
		if (wpid == 0 && current_conn->kill_cgi_time < current_time) //* Checks cgi kill time;
			keys_to_delete.push_back(it->first);
		else if (current_conn->close_conn_time < current_time) // * Checks conn based time;
			keys_to_delete.push_back(it->first);
			
	}
	for (size_t i = 0; i < keys_to_delete.size(); i++)
		close_conn(epoll_fd, keys_to_delete[i]);
}


/*
 * Listen to new connections and events within the connections;
*/
void WebServer::listen(void)
{
	int num_events = 0;
	int epoll_fd = this->epoll_fd;
	t_event *conn;
	t_event_data *event_data;

	while (is_running)
	{
		num_events = epoll_wait(epoll_fd, this->events.data(), this->max_events, 100 * 5);
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
		time_out(epoll_fd);
	}
	if (num_events < 0 && is_running)
		throw Error("Epoll_wait failed.");
}

/*Signal Handler*/
void WebServer::sig_handler(int sig)
{
	(void)sig;
	is_running = 0;
}

/*
 * Close a conn, and clean up the resources.
 */
void WebServer::close_conn(int epoll_fd, int fd)
{
	if (epoll_del_fd(epoll_fd, fd) < 0)
		throw Error("Epoll_ctl failed");
	close(fd); // *  Closes connection fd;
	// std::cout <<waitpid(this->streams[fd]->cgi_pid, NULL, WNOHANG) << std::endl;
	if (waitpid(this->streams[fd]->cgi_pid, NULL, WNOHANG) == 0)
	{
		std::cout << "Here we are" << std::endl;
		kill(this->streams[fd]->cgi_pid, SIGKILL);
	}
	delete this->streams[fd];
	this->streams.erase(fd);
	std::cout << "we've closed the connection on fd " << fd << std::endl;
}

/*Exception class*/
WebServer::Error::Error(const char *_msg) : msg(_msg) {}

const char *WebServer::Error::what() const throw()
{
	return (msg);
}
