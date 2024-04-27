#pragma once

#include "Lexer.hpp"
#include <utility>

typedef struct s_route {
	std::list<std::string> httpMethods;
	std::list<std::string> index;
	std::string rroot;
	bool dirListing;
} t_route;

typedef struct s_server {
	std::string host;
	int port;
	std::string serverName;
	std::string root;
	std::list<std::string> index;
	std::list<std::pair<int, std::string> > errorPages;
	int maxCBSize;
	int maxConn;
	std::list<t_route> route;
} t_server;

class Parser
{
private:
	std::list<t_server> serverNodes;
	std::map<int, bool> nodeCheck;
public:
	Parser();
	~Parser();
	
	bool	checkIndent(token token);
	void	parse(std::list<token> tokens);
};
