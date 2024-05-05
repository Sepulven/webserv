#pragma once

#include "Lexer.hpp"
#include <utility>

typedef struct s_route {
	std::string path;
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
	std::list<token>::iterator end;
	std::list<token>::iterator last;

	std::list<t_server> serverNodes;
	std::map<int, bool> nodeCheck;

	std::list<token>::iterator getLastTokenIt(std::list<token> tokens);
	void		printServerNodes(std::list<t_server>::iterator it);
	void 		clearLastOperation(int type);
	std::string getParam(token token);
	std::string getRoute(token token);
	bool		checkIndent(token token);

	/* Start of BNF syntax validator and 'serverNodes' 
	   composer based on syntax.txt. */

	bool configuration();
	bool configurationCase1();
	bool configurationCase2();

	bool serverBlock();
	bool serverBlockCase1();

	bool directives();
	bool directivesCase1();
	bool directivesCase2();
	bool directivesCase3();
	bool directivesCase4();
	bool directivesCase5();
	bool directivesCase6();

	bool blockDirs(int flag);
	bool blockDirsCase1(int flag);
	bool blockDirsCase2(int flag);

	template <typename T>
	bool parameterLst(T& container);
	template <typename T>
	bool parameterLstCase1(T& container);

public:
	Parser();
	~Parser();
	
	void	parse(std::list<token> tokens);
};
