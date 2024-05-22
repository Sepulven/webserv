# webserv
You will be able to test it with a real browser. HTTP is one of the most used protocol on internet. Knowing its arcane will be useful, even if you won't be working on a website. 


## Note (bind failed):
	`lsof -i :8080
	 kill [PID]`


## Note (leaks and errors):
	`valgrind --leak-check=yes --show-leak-kinds=all --undef-value-errors=yes ./webserv`

## Note (epoll performance compared to other polling APIs)
	`https://suchprogramming.com/epoll-in-3-easy-steps/`

# ps -p 1234 > /dev/null && echo true || echo false


	// get the route id to get access to 
	size_t i = 0;
	while (i < this->stream->server->routes.size() && stream->req->is_route != this->stream->server->routes[i].name)
		i++;
	if (i == this->stream->server->routes.size())
		this->stream->req->route_id = i - 1;
	else
		this->stream->req->route_id = i;