#include <ServerContext.hpp>

ServerContext::ServerContext(std::string _name, std::string _ip, int _port) 
: name(_name), ip(_ip), port(_port)
{
	this->max_events = 10;

	// Por enquanto não, mas no futuro sim
		//this->domain = getaddrinfo(NULL);//...gets the result;
	if (this->ip == "localhost")
		this->domain = INADDR_ANY;
}

ServerContext::~ServerContext()
{
	
}
