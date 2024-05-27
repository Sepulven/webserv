#include <ServerContext.hpp>
#include <Req.hpp>

/*
 * Builds the ServerContext from a node from the parser;
*/
ServerContext::ServerContext(t_server serverNode) 
{
	std::list<std::pair<std::string, std::string> >::iterator cgi_it = serverNode.cgi.begin();
	std::list<t_route>::iterator it = serverNode.route.begin();
	std::list<std::pair<int, std::string> >::iterator _it = serverNode.errorPages.begin();
	std::list<std::string>::iterator tmp;

	if (this->ip == "localhost" || this->ip != "localhost")
		this->domain = INADDR_ANY;

	// * Server info
	this->max_events = serverNode.maxConn;
	this->max_cb_size = serverNode.maxCBSize;
	this->port = serverNode.port;
	this->name = serverNode.serverName;
	this->ip = serverNode.host;

	for (; cgi_it != serverNode.cgi.end(); cgi_it++)
	{
		FILE* file = fopen(cgi_it->second.c_str(), "r");
		if (!file)
			throw ServerError("CGI path doesn't exist. Maybe the cgi is not installed");
		fclose(file);
		this->cgi_path.insert(std::make_pair(cgi_it->first, cgi_it->second));
	}

	// * Error pages
	for (;_it != serverNode.errorPages.end(); _it++)
		this->error_pages.insert(std::make_pair(_it->first, _it->second));

	// * Routes
	for (; it != serverNode.route.end(); it++)
	{
		t_location new_location;
		new_location.name = it->path;
		std::cout << "route root: " << it->rroot << std::endl;
		if (Req::get_path_type("." + it->rroot) != _DIRECTORY)
			throw ServerError("Invalid route root.");
		if (it->rroot.size() == 1 && it->rroot[0] == '/')
			new_location.root = "/.";
		else
			new_location.root = it->rroot;
		new_location.redirect = it->redir;
		// std::cout << "dir listing 0: " << it->dirListing << std::endl;
		new_location.dir_listing = it->dirListing;

		// * Http Allowed methods
		tmp = it->httpMethods.begin();
		for (;tmp != it->httpMethods.end(); tmp++)
			new_location.http_methods.push_back(*tmp);

		// * List of index
		tmp = it->index.begin();
		for (;tmp != it->index.end(); tmp++)
			new_location.index.push_back(*tmp);

		// * Pushes the new route to the server context;
		this->routes.push_back(new_location);
	}

	// * main route
	t_location server_location;
	server_location.name = ".";
	if (serverNode.root.size() == 1 && serverNode.root[0] == '/')
		server_location.root = "/.";
	else
		server_location.root = serverNode.root;
	if (Req::get_path_type("." + server_location.root) != _DIRECTORY)
			throw ServerError("Invalid server root.");
	server_location.redirect = std::string();
	for (std::list<std::string>::iterator it = serverNode.index.begin(); it != serverNode.index.end(); it++)
		server_location.index.push_back(*it);
	for (std::list<std::string>::iterator it = serverNode.httpMethods.begin(); it != serverNode.httpMethods.end(); it++)
		server_location.http_methods.push_back(*it);

	server_location.dir_listing = serverNode.dirListing; // ! NEEDS TO BE CHANGED LATER.
	// std::cout << "dir listing root: " << server_location.dir_listing << std::endl;

	this->routes.push_back(server_location);
}

/*
 * Frees the t_event_data pointer allocated in the init server function;
*/
ServerContext::~ServerContext()
{
	delete epoll_event_info;
}
