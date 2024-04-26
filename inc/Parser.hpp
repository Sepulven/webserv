#pragma once

#include "Lexer.hpp"
#include <utility>

enum nodeType {
	HOST,
	PORT,
	NAME,
	ROOT,
	INDEX,
	ERROR_PAGE,
	MAX_CBSIZE,
	MAX_CONN,
	METHOD,
	DIR_LISTING
};

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
	std::list<token> tokens;
	std::list<t_server> serversConf;
public:
	Parser();
	~Parser();

	void parse(std::list<token> tokens);

};
