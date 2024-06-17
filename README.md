# 	WEBSERV

## What is a Web Server?

A web server is software that understands URLs and the HTTP protocol. It stores content and 
delivers it depending on the user's request as an HTTP response. For example, the content could be anything from images and videos to HTML pages and JSON files.


## Why did we build a Web Server?

Building a Web Server is the foundation for understanding web technologies.
It's a great tool to dive deeper into how networking in browsers works and see what is happening behind the scenes. It broadens our perspectives and skills and enhances our educational values through a demystifying complex system process.
In the end, we achieve a better knowledge of HTTP protocol, sockets, request and response cycles architecture, software architecture and so on.

 

## Installation Process

In your terminal run:
~~~bash
cd ~/Desktop
git clone https://github.com/Sepulven/webserv.git
cd webserv
make
./webserv config.yml
~~~
We do not advise you to run it in any virtual terminal, for example, VS Code's terminal, it may create zombie child processes when you finish the execution which will block the port.

In case that happens, you need to find the process that uses the PORT and kills it through its PID.

In your terminal run:
~~~bash
lsof -i :[PORT]
kill -9 [PID]
~~~

When testing it use Firefox as a browser. Where else you will run into unexpected behaviors.

In your terminal run:

~~~bash
firefox 127.0.0.1:8080/
~~~

## Development Environment

Operating System:

	**Unix/Linux**
	Distributor ID:	Ubuntu
	Description:	Ubuntu 22.04.4 LTS
	Release:	22.04
	Codename:	jammy

Compiler:
	
	c++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0

Browser:

	firefox --version


## How to use it
### Understanding the config file
## File structure

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
