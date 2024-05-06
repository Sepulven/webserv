#include "../../inc/parser/Parser.hpp"

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

s_server::s_server() {
	this->host = std::string();
	this->port = -1;
	this->maxCBSize = -1;
	this->maxConn = -1;
	this->serverName = std::string();
	this->root = std::string();
}

void Parser::resetParam(int type, int identLevel) {
	std::cout << "CLEARED VALUES ON TYPE: " << type << " IDENT LVL: " << identLevel << std::endl;
	if (type == INDEX) {
		if (identLevel == 1)
			serverNodes.back().index.pop_back();
		if (identLevel == 2)
			serverNodes.back().route.back().index.pop_back();
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
	if (type == ROOT)
		serverNodes.back().root = std::string();
}

std::list<token>::iterator Parser::getLastTokenIt(std::list<token> tokens) {
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
	size_t j = token.content.find_first_of(':') - 1;
	while (std::isspace(token.content[j]))
		j--;
	return token.content.substr(i, j - i + 1);
}