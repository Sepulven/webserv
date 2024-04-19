
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

	std::string end = "\r\n\r\n";

	while (!aux.find(end))
    {
        buffer[bytes_read] = '\0';
        aux =+ buffer;
        bytes_read = read(fd, buffer, sizeof(buffer));
    }
	buffer[bytes_read] = 0;
	str = aux;

	if (aux.find(end))
	{
		this->requests.insert(make_pair(fd, aux));
		std::cout << "check fim\n";
		if (epoll_out_fd(epoll_fd, fd, event))
			throw Error("Epoll_ctl failed");
	}
	else
	{
		std::cout << "file not supported 0\n";
		throw Error("");
	}
}


void WebServer::send_response(int epoll_fd, int fd, struct epoll_event event)
{
	Req req(fd, (this->requests)[fd]);
	req.process_request();

	this->requests.erase(fd);
	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nSuccess!";
	write(fd, response.c_str(), response.size()); // send file
	// New response object
	if (epoll_in_fd(epoll_fd, fd, event) < 0)
		throw Error("Epoll_ctl failed");

}
