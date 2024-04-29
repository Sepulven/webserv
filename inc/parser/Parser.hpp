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
	std::list<token>::iterator it;
	std::list<t_server> serverNodes;
	std::map<int, bool> nodeCheck;

	std::string getParam(token token);
	bool		checkIndent(token token);

	/* Start of BNF syntax validator and 'serverNodes' 
	   composer based on syntax.txt. */

	bool configuration();
	bool configurationCase1();
	bool configurationCase2();

	bool

public:
	Parser();
	~Parser();
	
	void	parse(std::list<token> tokens);
};
