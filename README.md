# webserv

## Installation process
### Requisition


### Notes:
- It is only compitible with Firefox
- The webserv must run on bash. VSCode's terminal create zombie processes that takes the port.</br>
    **Kill the process:**</br>
    ~~~bash
    lsof -i :[PORT]
    kill -9 [PID]
    ~~~


## What is a webserv?

A webserv is a software that understands URLs and the http protocol. It stores content and 
delivers it depending on the user request as a http response. For example, the content could be anything from image and videos to html pages and json files.

## Why we builted a webserv?

Building an webserv is the foundation for a good understanding of the whole web.
It's a great tool to


## What challenges were faced?


## If I want to build an webserv how should I begin?

## Performance
siege
single threaded


## References

 - [Epoll Performance](https://suchprogramming.com/epoll-in-3-easy-steps/)
 - [How NodeJs works](https://www.youtube.com/watch?v=wB9tIg209-8)
 - [Internal Architecture of NodeJs](https://youtu.be/vyPzHBKa88w?si=7xhh935QGnAGgCjM)

## Main Tests
    Evaluation sheet

## How it works?
### **Overview**
    Single threaded
    CGI
    Parsing
    Event handling with epoll
[... here comes the whole explanation]



----



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