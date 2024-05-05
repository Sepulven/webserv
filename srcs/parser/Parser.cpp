#include "../../inc/parser/Parser.hpp"

Parser::Parser() {}

Parser::~Parser() {}

void Parser::printServerNodes(std::list<t_server>::iterator it) {
	if (it == serverNodes.end())
		return ;
	static int i;
	if (i == 0)
		std::cout << std::endl << "============Parser============" << std::endl;
	std::cout << "Server: " << i++ << std::endl;
	std::cout << "    Host: " << it->host << std::endl;
	std::cout << "    Port: " << it->port << std::endl;
	std::cout << "    Server name: " << it->serverName << std::endl;
	std::cout << "    Root: " << it->root << std::endl;
	std::cout << "    Max client body size: " << it->maxCBSize << std::endl;
	std::cout << "    Max connections: " << it->maxConn << std::endl;
	std::cout << "    Index: ";
	for (std::list<std::string>::iterator tmp = it->index.begin(); tmp != it->index.end(); tmp++)
		std::cout << *tmp << " ";
	std::cout << std::endl << "    Error pages:" << std::endl;
	for (std::list<std::pair<int, std::string> > ::iterator tmp = it->errorPages.begin(); tmp != it->errorPages.end(); tmp++)
		std::cout << "        "<< tmp->first << " " << tmp->second << std::endl;
	for (std::list<t_route>::iterator tmp = it->route.begin(); tmp != it->route.end(); tmp++) {
		std::cout << "    Route: " << tmp->path << std::endl;
		std::cout << "        Root: " << tmp->rroot << std::endl;
		std::cout << "        HTTP methods: ";
		for (std::list<std::string>::iterator tmp2 = tmp->httpMethods.begin(); tmp2 != tmp->httpMethods.end(); tmp2++)
			std::cout << *tmp2 << " ";
		std::cout << std::endl << "        Index: ";
		for (std::list<std::string>::iterator tmp2 = tmp->index.begin(); tmp2 != tmp->index.end(); tmp2++)
			std::cout << *tmp2 << " ";
		std::cout << std::endl << "        Directory listing: " << (tmp->dirListing ? "true" : "false") << std::endl;
	}
	printServerNodes(++it);
}

bool Parser::checkIndent(token token) {
	if (token.identLevel < 0)
		return false;
	return true;
}

std::string Parser::getParam(token token) {
	size_t i = token.content.find_first_of(':') + 1;
	while (std::isspace(token.content[i]))
		i++;
	size_t j = token.content.length() - 1;
	while (std::isspace(token.content[j]))
		j--;
	return token.content.substr(i, j - i + 1);
}

std::string Parser::getRoute(token token) {
	if (token.content.find_first_of('/') == std::string::npos)
		return std::string();
	size_t i = token.content.find_first_of('/') + 1;
	size_t j = token.content.find_first_of(':') - 1;
	while (std::isspace(token.content[j]))
		j--;
	return token.content.substr(i, j - i + 1);
}

void Parser::parse(std::list<token> tokens) {
	for (std::list<token>::iterator it = tokens.begin();
	it != tokens.end(); it++) {
		if (!checkIndent(*it))
			throw Lexer::Error("Bad identation in config file.");
	}
	this->it = tokens.begin();
	this->end = tokens.end();
	this->last = getLastTokenIt(tokens);

	configuration();
	printServerNodes(serverNodes.begin());
}

// TODO: HANDLE ALREADY FILLED PARAMETERS.

// Start of BNF syntax validator and 'serverNodes' composer based on syntax.txt.

// Main <configuration> function.

bool Parser::configuration() {
	std::list<token>::iterator tmp = it;
	if (configurationCase1())
		return true;
	it = tmp;
	if (configurationCase2())
		return true;
	return false;
}

// <server_block> <configuration>

bool Parser::configurationCase1() {
	if (it == end)
		return false;
	std::cout << "\033[31m" << "Entered configurationCase1 with: " << it->content << " | type: " << it->type << "\033[0m" << std::endl;
	if (it->type != SERVERBLOCK || it->identLevel != 0)
		return false;
	it++;
	t_server newServerNode;
	serverNodes.push_back(newServerNode);
	if (!serverBlock())
		return serverNodes.pop_back(), false;
	if (!configuration())
		return serverNodes.pop_back(), false;
	return true;
}

// <server_block>

bool Parser::configurationCase2() {
	if (it == end)
		return false;
	std::cout << "\033[31m" << "Entered configurationCase2 with: " << it->content << " | type: " << it->type << "\033[0m" << std::endl;
	if (it->type != SERVERBLOCK || it->identLevel != 0)
		return false;
	it++;
	t_server newServerNode;
	serverNodes.push_back(newServerNode);
	if (!serverBlock())
		return serverNodes.pop_back(), false;
	if (it != end)
		return serverNodes.pop_back(), false;
	return true;
}

// Main <server_block> function.

bool Parser::serverBlock() {
	if (serverBlockCase1())
		return true;
	return false;
}

// <directives>

bool Parser::serverBlockCase1() {
	std::cout << "Entered serverBlockCase1 with: " << it->content << " | type: " << it->type << std::endl;
	if (it->identLevel != 1)
		return false;
	if (!directives())
		return false;
	return true;
}

// Main <directives> function.

bool Parser::directives() {
	std::list<token>::iterator tmp = it;
	if (directivesCase1())
		return true;
	it = tmp;
	if (directivesCase2())
		return true;
	it = tmp;
	if (directivesCase3())
		return true;
	it = tmp;
	if (directivesCase4())
		return true;
	it = tmp;
	if (directivesCase5())
		return true;
	it = tmp;
	if (directivesCase6())
		return true;
	return false;
}

// <block_dirs> [path] <directives> (ROUTE)

bool Parser::directivesCase1() {
	if (it == end)
		return false;
	std::cout << "Entered directivesCase1 with: " << it->content << " | type: " << it->type << std::endl;
	if (it->type != ROUTE)
		return false;
	std::string routePath = getRoute(*it);
	if (routePath.empty())
		return false;
	t_route newRoute;
	serverNodes.back().route.push_back(newRoute);
	serverNodes.back().route.back().path = routePath;
	it++;
	if (!blockDirs(ROUTE))
		return serverNodes.back().route.pop_back(), false;
	if (!directives())
		return serverNodes.back().route.pop_back(), false;
	return true;
}

// <block_dirs> <directives>

bool Parser::directivesCase2() {
	if (it == end)
		return false;
	std::cout << "Entered directivesCase2 with: " << it->content << " | type: " << it->type << std::endl;
	if (it->type != LISTEN && it->type != ERROR_PAGE_BLOCK)
		return false;
	int flag = it->type == LISTEN ? LISTEN : ERROR_PAGE_BLOCK;
	it++;
	if (!blockDirs(flag)) {  // !!!!!!!!!1
		std::cout << "DEBUG1" << std::endl;
		return false;
	} 
	if (!directives()) {
		std::cout << "DEBUG2" << std::endl;
		if (it != end && it->type == LISTEN) // !!!!!!!!!!!!!!
			return clearLastOperation(it->type), false;
	}
	return true;
}

// <simple_dir> <directives>

bool Parser::directivesCase3() {
	if (it == end)
		return false;
	std::cout << "Entered directivesCase3 with: " << it->content << " | type: " << it->type << std::endl;
	if (it->type != NAME && it->type != ROOT && it->type != INDEX && 
	it->type != MAX_CBSIZE && it->type != MAX_CONN)
		return false;
	if (!parameterLst(serverNodes))
		return false;
	if (!directives()) {
		std::list<token>::iterator tmp = it;
		tmp = tmp == end ? last : --tmp;
		return clearLastOperation(tmp->type), false;
	}
	return true;
}

// <block_dirs> [path] *

bool Parser::directivesCase4() {
	if (it == end)
		return false;
	std::cout << "Entered directivesCase4 with: " << it->content << " | type: " << it->type << std::endl;
	if (it->type != ROUTE)
		return false;
	std::string routePath = getRoute(*it);
	if (routePath.empty())
		return false;
	t_route newRoute;
	serverNodes.back().route.push_back(newRoute);
	serverNodes.back().route.back().path = routePath;
	it++;
	if (!blockDirs(ROUTE)) {
		serverNodes.back().route.pop_back();
		return false;
	}
	return true;
}

// <block_dirs>

bool Parser::directivesCase5() {
	if (it == end)
		return false;
	std::cout << "Entered directivesCase5 with: " << it->content << " | type: " << it->type << std::endl;
	if (it->type != LISTEN && it->type != ERROR_PAGE_BLOCK)
		return false;
	int flag = it->type == LISTEN ? LISTEN : ERROR_PAGE_BLOCK;
	it++;
	if (!blockDirs(flag))
		return false;
	return true;
}

// <simple_dir>

bool Parser::directivesCase6() {
	if (it == end)
		return false;
	std::cout << "Entered directivesCase6 with: " << it->content << " | type: " << it->type << std::endl;
	if (it->type != NAME && it->type != ROOT && it->type != INDEX && 
	it->type != MAX_CBSIZE && it->type != MAX_CONN) {
		return false;
	}
	if (!parameterLst(serverNodes))
		return false;
	return true;
}

// Main <block_dirs> function.

bool Parser::blockDirs(int flag) {
	std::list<token>::iterator tmp = it;
	if (blockDirsCase1(flag))
		return true;
	it = tmp;
	if (blockDirsCase2(flag))
		return true;
	return false;
}

// <parameter_lst> <block_dirs>

bool Parser::blockDirsCase1(int flag) {
	std::list<token>::iterator check = it;
	if (it == end || ++check == end)
		return false;
	std::cout << "Entered blockDirsCase1 with: " << it->content << " | type: " << it->type << std::endl;
	if (it->identLevel != 2)
		return false;
	if (flag == ROUTE)
		if (!parameterLst(serverNodes.back().route))
			return false;
	if (flag == LISTEN)
		if (!parameterLst(serverNodes))
			return false;
	if (flag == ERROR_PAGE_BLOCK)
		if (!parameterLst(serverNodes.back().errorPages))
			return false;
	if (!blockDirs(flag)) {
		std::list<token>::iterator tmp = it;
		tmp = tmp == end ? last : --tmp;
		return clearLastOperation(tmp->type), false;
	}
	return true;
}

// <parameter_lst>

bool Parser::blockDirsCase2(int flag) {
	if (it == end)
		return false;
	std::cout << "Entered blockDirsCase2 with: " << it->content << " | type: " << it->type << std::endl;
	if (it->identLevel != 2)
		return false;
	if (flag == ROUTE)
		if (!parameterLst(serverNodes.back().route))
			return false;
	if (flag == LISTEN)
		if (!parameterLst(serverNodes))
			return false;
	if (flag == ERROR_PAGE_BLOCK)
		if (!parameterLst(serverNodes.back().errorPages))
			return false;
	return true;
}

// todo: NEEDS PARAMETER RESET FUNCTIONS.

// Main <parameter_lst> function.

template<typename T>
bool Parser::parameterLst(T& container) {
	if (parameterLstCase1(container))
		return true;
	return false;
}

// [parameters]

template<typename T>
bool Parser::parameterLstCase1(T& container) {
	if (it == end)
		return false;
	std::cout << "\033[32m" << "Entered parameterLstCase1 with: " << it->content << " | type: " << it->type << "\033[0m" << std::endl;
	if (getParam(*it).empty())         // Need to check with ppl if there are default values.
		return false;
	if (it->type == HOST)
		container.back().host = getParam(*it);
	if (it->type == PORT)
		container.back().port = atoi(getParam(*it).c_str());
	if (it->type == INDEX)
		container.back().index.push_back(getParam(*it));
	if (it->type == ROOT)
		container.back().root = getParam(*it);
	if (it->type == NAME)
		container.back().serverName = getParam(*it);
	if (it->type == MAX_CBSIZE)
		container.back().maxCBSize = atoi(getParam(*it).c_str()); // Needs mb to byte conversion function.
	if (it->type == MAX_CONN)
		container.back().maxConn = atoi(getParam(*it).c_str());
	it++;
	return true;
}

// [parameters] ROUTE

template <>
bool Parser::parameterLstCase1<std::list<t_route> >(std::list<t_route>& container) {
	if (it == end)
		return false;
	std::cout << "\033[32m" <<  "Entered parameterLstCase1 ROUTE with: " << it->content << " | type: " << it->type << "\033[0m" << std::endl;
	if (getParam(*it).empty())
		return false;
	if (it->type == INDEX)
		container.back().index.push_back(getParam(*it)); // Handle multiple params.
	else if (it->type == ROOT)
		container.back().rroot = getParam(*it);
	else if (it->type == METHOD)
		container.back().httpMethods.push_back(getParam(*it));
	else if (it->type == DIR_LISTING) {
		if (getParam(*it) == "on")
			container.back().dirListing = true;
		else if (getParam(*it) == "off")
			container.back().dirListing = false;
	}
	else
		return false;
	it++;
	return true;
}

template <>
bool Parser::parameterLstCase1<std::list<std::pair<int, std::string> > >(std::list<std::pair<int, std::string> >& container) {
	(void)container;
	if (it == end)
		return false;
	std::cout << "\033[32m" <<  "Entered parameterLstCase1 ERROR_PAGE with: " << it->content << " | type: " << it->type << "\033[0m" << std::endl;
	int code = atoi(it->content.c_str());
	if (code == 0)
		return false;
	container.push_back(std::make_pair(code, getParam(*it)));
	it++;
	return true;
}

// CHECK ITERATION, TOKENIZATION NOT IN [name]->[param] FORM.
// ACTUAL TOKENIZATION: [name][param]->[name][param].