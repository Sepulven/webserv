#include <ServerContext.hpp>


ServerContext::ServerContext(t_server serverNode) 
{
	if (this->ip == "localhost")
		this->domain = INADDR_ANY;
	this->max_events = serverNode.maxConn;
	this->max_cb_size = serverNode.maxCBSize;
	this->port = serverNode.port;
	this->name = serverNode.serverName;
	this->ip = serverNode.host;
	for (std::list<std::pair<int, std::string> >::iterator it = serverNode.errorPages.begin();
	it != serverNode.errorPages.end(); it++)
		this->error_pages.insert(std::make_pair(it->first, it->second));
	for (std::list<t_route>::iterator it = serverNode.route.begin();
	it != serverNode.route.end(); it++) {
		t_location new_location;
		new_location.path = it->path;
		new_location.root = it->rroot;
		new_location.redirect = it->redir;
		new_location.dir_listing = it->dirListing;
		for (std::list<std::string>::iterator tmp = it->httpMethods.begin();
		tmp != it->httpMethods.end(); tmp++)
			new_location.http_methods.push_back(*tmp);
		for (std::list<std::string>::iterator tmp = it->index.begin();
		tmp != it->index.end(); tmp++)
			new_location.index.push_back(*tmp);
		this->routes.push_back(new_location);
	}
}

ServerContext::~ServerContext()
{
	
}
