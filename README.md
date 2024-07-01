# 	WEBSERV

## What is a Web Server?

A web server is software that understands URLs and the HTTP protocol. It stores content and 
delivers it depending on the user's request as an HTTP response. For example, the content could be anything from images and videos to HTML pages and JSON files.


## Why did we build a Web Server?

Building a Web Server is the foundation for understanding web technologies.
It's a great tool to dive deeper into how networking in browsers works and see what is happening behind the scenes. It broadens our perspectives and skills and enhances our educational values through a demystifying complex system process.
In the end, we achieve a better knowledge of HTTP protocol, sockets, request and response cycles, software architecture and so on.

 
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

Create the executable file as explained before and create a config file following the rules of the config file.
It will server locally all of the files requested and execute all of the defined scripts (nodejs, bash, php, python, ..., etc.).

### Understanding the config file

The config file is defined in a YAML like file. Its structure is very similar except for that fact we can have multiple definition of the safe variable they are going to be stored as array.
For example:
```YAML
server:
	[...]
server:
	[...]
```
### Definitions
- listen:
	- host : string | number
 	- port: number
- server_name: string
- maxcbsize: string (megabe bytes)
- max_conn: number
- root: path
- index: path
- http_methods: list of strings
- error_pages:
	- number : path
- cgi:
  - string : path
- route path:
	-  root: path
  	- redirect: path
  	- index: path
	-  dir_listing: on | off
 	-  http_methods: list of strings
  
## File structure

The whole project is structured into pieces for each piece you have the declaration and the implementation, namely a .h and .cpp respectively.
Each feature is a folder defined, and all of its extra dependencies go into the ```__utils``` folder.

Every utils file starts with ```___``` and ends with ```utils``` followed by its extension for better identification.

## If I want to build an webserv in C++98 how should I begin?

First understanding the basis of a client and a server is a good start.
Next how the HTTP request and response cycle works.
After that how file descriptors can be used as web sockets.
Then how to syncronize fd's events with epoll or any of its variants.
Finally, use all of the tools C++98 has to give to manage the requests and responses, you're going to see 
that it is pretty much managing I/O operation with file descriptors.


## Test performance and availability with Siege

Siege is an http load tester and benchmarking utility.

Install siege with:
```bash
sudo apt install siege
```
[Siege Manual](https://linux.die.net/man/1/siege)

## Main Tests
    Evaluation sheet

## How it works?
### **Overview**
    Single threaded
    CGI
    Parsing
    Event handling with epoll
[... here comes the whole explanation]



## References

 - [Epoll Performance](https://suchprogramming.com/epoll-in-3-easy-steps/)
 - [How NodeJs works](https://www.youtube.com/watch?v=wB9tIg209-8)
 - [Internal Architecture of NodeJs](https://youtu.be/vyPzHBKa88w?si=7xhh935QGnAGgCjM)

