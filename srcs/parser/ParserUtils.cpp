#include "Parser.hpp"

void Parser::printServerNodes(std::list<t_server>::iterator it) {
	if (it == serverNodes.end())
		return ;
	static int i;
	if (i == 0)
		std::cout << std::endl << "============Parser============" << std::endl;
	std::cout << "Server: " << i++ + 1 << std::endl;
	std::cout << "    Host: " << it->host << std::endl;
	std::cout << "    Port: " << it->port << std::endl;
	std::cout << "    Server name: " << it->serverName << std::endl;
	std::cout << "    Root: " << it->root << std::endl;
	std::cout << "    Redirection: " << it->redir << std::endl;
	std::cout << "    Max client body size: " << it->maxCBSize << std::endl;
	std::cout << "    Max connections: " << it->maxConn << std::endl;
	std::cout << "    Directory listing: " << (it->dirListing == 1 ? "true" : "false") << std::endl;
	std::cout << "    Index: ";
	for (std::list<std::string>::iterator tmp = it->index.begin(); tmp != it->index.end(); tmp++)
		std::cout << *tmp << "$ ";
	std::cout << std::endl << "    HTTP methods: ";
	for (std::list<std::string>::iterator tmp = it->httpMethods.begin(); tmp != it->httpMethods.end(); tmp++)
		std::cout << *tmp << "$ ";
	std::cout << std::endl << "    Error pages:" << std::endl;
	for (std::list<std::pair<int, std::string> > ::iterator tmp = it->errorPages.begin(); tmp != it->errorPages.end(); tmp++)
		std::cout << "        "<< tmp->first << " " << tmp->second << std::endl;
	std::cout << "    CGI:" << std::endl;
	for (std::list<std::pair<std::string, std::string> > ::iterator tmp = it->cgi.begin(); tmp != it->cgi.end(); tmp++)
		std::cout << "        "<< tmp->first << " " << tmp->second << std::endl;
	for (std::list<t_route>::iterator tmp = it->route.begin(); tmp != it->route.end(); tmp++) {
		std::cout << "    Route: " << tmp->path << std::endl;
		std::cout << "        Root: " << tmp->rroot << std::endl;
		std::cout << "        Redirection: " << tmp->redir << std::endl;
		std::cout << "        HTTP methods: ";
		for (std::list<std::string>::iterator tmp2 = tmp->httpMethods.begin(); tmp2 != tmp->httpMethods.end(); tmp2++)
			std::cout << *tmp2 << " ";
		std::cout << std::endl << "        Index: ";
		for (std::list<std::string>::iterator tmp2 = tmp->index.begin(); tmp2 != tmp->index.end(); tmp2++)
			std::cout << *tmp2 << " ";
		std::cout << std::endl << "        Directory listing: " << (tmp->dirListing == 1 ? "true" : "false") << std::endl;
	}
	printServerNodes(++it);
}

s_server::s_server() {
	this->host = std::string();
	this->redir = std::string();
	this->port = -1;
	this->maxCBSize = -1;
	this->maxConn = -1;
	this->dirListing = -1;
	this->serverName = std::string();
	this->root = std::string();
}

s_route::s_route() {
	this->rroot = std::string();
	this->redir = std::string();
	this->dirListing = -1;
	this->path = std::string();
}

void Parser::resetParam(int type, int identLevel) {
	// std::cout << "CLEARED VALUES ON TYPE: " << type << " IDENT LVL: " << identLevel << std::endl;
	if (type == INDEX) {
		if (identLevel == 1)
			serverNodes.back().index.clear();
		if (identLevel == 2)
			serverNodes.back().route.back().index.clear();
	}
	if (type == METHOD) {
		if (identLevel == 1)
			serverNodes.back().httpMethods.clear();
		if (identLevel == 2)
			serverNodes.back().route.back().httpMethods.clear();
	}
	if (type == LISTEN) {
		serverNodes.back().host = std::string();
		serverNodes.back().port = -1;
	}
	if (type == HOST)
		serverNodes.back().host = std::string();
	if (type == PORT)
		serverNodes.back().port = -1;
	if (type == -1)
		serverNodes.back().errorPages.pop_back();
	if (type == NAME)
		serverNodes.back().serverName = std::string();
	if (type == ROOT) {
		if (identLevel == 1)
			serverNodes.back().root = std::string();
		if (identLevel == 2)
			serverNodes.back().route.back().rroot = std::string();
	}
	if (type == DIR_LISTING) {
		if (identLevel == 1)
			serverNodes.back().dirListing = -1;
		if (identLevel == 2)
			serverNodes.back().route.back().dirListing = -1;
	}
	if (type == MAX_CBSIZE)
		serverNodes.back().maxCBSize = -1;
	if (type == MAX_CONN)
		serverNodes.back().maxConn = -1;
	if (type == REDIRECT) {
		if (identLevel == 2)
			serverNodes.back().route.back().redir = std::string();
		else if (identLevel == 1)
			serverNodes.back().redir = std::string();
	}
	if (type == CGI_PARAM)
		serverNodes.back().cgi.pop_back();
}

std::list<token>::iterator Parser::getLastTokenIt(std::list<token> tokens) {
	std::list<token>::iterator last;
	for (std::list<token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		last = it;
	}
	return last;
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
	size_t y = token.content.find_last_of('/');
	if (y != std::string::npos) {
		while (y > i) {
			if (std::isspace(token.content[y--]))
				return std::string();
		}
	}
	size_t j = token.content.find_first_of(':') - 1;
	for (size_t k = j; k != i; k--)
		if (std::isspace(token.content[k]))
			return std::string();
	while (std::isspace(token.content[j]))
		j--;
	return token.content.substr(i, j - i + 1);
}

long long Parser::convertToByte(std::string str) {
	int count = 0;
	for (std::string::iterator it = str.begin(); it != str.end(); it++) {
		if (std::isalpha(*it))
			count++;
	}
	long long number = std::atoll(str.c_str());
	char multiplier = str[str.size() - 1];
	if (count > 1)
		return -1;
	if (multiplier == 'k')
		return number * 1024;
	else if (multiplier == 'm')
		return number * 1024 * 1024;
	else if (count == 0)
		return number;
	else {
		throw Lexer::Error("Invalid client body size unit.");
		return -1;
	}
}

void Parser::pushBackMultipleParams(std::list<std::string>& list, std::string str) {
	size_t i = str.find_first_of(" \t\v\f\r");
	if (i == std::string::npos) {
		list.push_back(str.substr(0, str.size()));
			return ;
	}
	list.push_back(str.substr(0, i));
	while (std::isspace(str[i]))
		i++;
	str = str.substr(i, str.size());
	pushBackMultipleParams(list, str);
}

std::list<t_server>::iterator Parser::getServerNodesIt(void) {
	return serverNodes.begin();
}

std::list<t_server> Parser::getServerNodes(void) {
	return serverNodes;
}