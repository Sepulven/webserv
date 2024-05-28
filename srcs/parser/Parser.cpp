#include "Parser.hpp"

Parser::Parser() {}

Parser::~Parser() {}

void Parser::parse(std::list<token> tokens) {
	for (std::list<token>::iterator it = tokens.begin();
	it != tokens.end(); it++) {
		if (!checkIndent(*it))
			throw Lexer::Error("Bad identation in config file.");
	}
	this->it = tokens.begin();
	this->end = tokens.end();
	this->tokens = tokens;
	if (!configuration())
		std::cout << std::endl << "FAILED" << std::endl;
}

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
	if (routePath.empty() || it->content.find_first_of(':') == std::string::npos)
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
	if (it->type != LISTEN && it->type != ERROR_PAGE_BLOCK && it->type != CGI)
		return false;
	int flag = -1;
	if (it->type == LISTEN)
		flag = LISTEN;
	else if (it->type == ERROR_PAGE_BLOCK)
		flag = ERROR_PAGE_BLOCK;
	else if (it->type == CGI)
		flag = CGI;
	it++;
	if (!blockDirs(flag)) {  // !!!!!!!!!
		std::cout << "DEBUG1" << std::endl;
		return false;
	} 
	if (!directives()) {
		std::cout << "DEBUG2" << std::endl;
		if (it != end && it->type == LISTEN) // !!!!!!!!!!!!!!
			return resetParam(it->type, it->identLevel), false;
	}
	return true;
}

// <simple_dir> <directives>

bool Parser::directivesCase3() {
	std::list<token>::iterator check = it;
	if (it == end || ++check == end) // ! MIGHT NOT BE A FIX
		return false;
	std::cout << "Entered directivesCase3 with: " << it->content << " | type: " << it->type << std::endl;
	if (it->type != NAME && it->type != ROOT && it->type != INDEX && 
	it->type != MAX_CBSIZE && it->type != MAX_CONN && it->type != DIR_LISTING && it->type != METHOD
	&& it->type != REDIRECT)
		return false;
	if (it->identLevel != 1)
		return false;
	if (!parameterLst(serverNodes))
		return false;
	if (!directives()) {
		std::list<token>::iterator tmp = it;
		tmp = tmp == end ? getLastTokenIt(tokens) : --tmp;
		return resetParam(tmp->type, tmp->identLevel), false;
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
	if (routePath.empty() || it->content.find_first_of(':') == std::string::npos)
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
	if (it->type != LISTEN && it->type != ERROR_PAGE_BLOCK && it->type != CGI)
		return false;
	int flag = -1;
	if (it->type == LISTEN)
		flag = LISTEN;
	else if (it->type == ERROR_PAGE_BLOCK)
		flag = ERROR_PAGE_BLOCK;
	else if (it->type == CGI)
		flag = CGI;
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
	it->type != MAX_CBSIZE && it->type != MAX_CONN && it->type != DIR_LISTING && it->type != METHOD
	&& it->type != REDIRECT)
		return false;
	if (it->identLevel != 1)
		return false;
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
	if (flag == CGI)
		if (!parameterLst(serverNodes.back().cgi))
			return false;
	if (!blockDirs(flag)) {
		std::list<token>::iterator tmp = it;
		tmp = tmp == end ? getLastTokenIt(tokens) : --tmp;
		return resetParam(tmp->type, tmp->identLevel), false;
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
	if (flag == LISTEN && (it->type == HOST || it->type == PORT))
		if (!parameterLst(serverNodes))
			return false;
	if (flag == ERROR_PAGE_BLOCK)
		if (!parameterLst(serverNodes.back().errorPages))
			return false;
	if (flag == CGI)
		if (!parameterLst(serverNodes.back().cgi))
			return false;
	return true;
}

// <parameter_lst> function.

template<typename T>
bool Parser::parameterLst(T& container) {
	if (parameterLstCase1(container))
		return true;
	return false;
}

// [parameters] DEFAULT

template<typename T>
bool Parser::parameterLstCase1(T& container) {
	if (it == end)
		return false;
	std::cout << "\033[32m" << "Entered parameterLstCase1 with: " << it->content << " | type: " << it->type << "\033[0m" << std::endl;
	if (getParam(*it).empty())         // Need to check with ppl if there are default values.
		return false;
	if (it->type == HOST && container.back().host.empty())
		container.back().host = getParam(*it);
	else if (it->type == METHOD && container.back().httpMethods.empty())
		pushBackMultipleParams(container.back().httpMethods, getParam(*it));
	else if (it->type == REDIRECT && container.back().redir.empty())
		container.back().redir = getParam(*it);
	else if (it->type == PORT && container.back().port == -1)
		container.back().port = atoi(getParam(*it).c_str());
	else if (it->type == INDEX && container.back().index.empty())
		pushBackMultipleParams(container.back().index, getParam(*it));
	else if (it->type == ROOT && container.back().root.empty())
		container.back().root = getParam(*it);
	else if (it->type == NAME && container.back().serverName.empty())
		container.back().serverName = getParam(*it);
	else if (it->type == MAX_CBSIZE && container.back().maxCBSize == -1)
		container.back().maxCBSize = convertToByte(getParam(*it).c_str());
	else if (it->type == MAX_CONN && container.back().maxConn == -1 && getParam(*it).find_first_not_of("1234567890") == std::string::npos)
		container.back().maxConn = atoi(getParam(*it).c_str());
	else if (it->type == DIR_LISTING && container.back().dirListing == -1) {
		if (getParam(*it) == "on")
			container.back().dirListing = 1;
		else if (getParam(*it) == "off")
			container.back().dirListing = 0;
		else
			return false;
	}
	else
		return false;
	it++;
	return true;
}

// [parameters] ROUTE

template <>
bool Parser::parameterLstCase1<std::list<t_route> >(std::list<t_route>& container) {
	if (it == end)
		return false;
	std::cout << "\033[32m" <<  "Entered parameterLstCase1 ROUTE with: " << it->content << " | type: " << it->type << "\033[0m" << std::endl;
	if (getParam(*it).empty()) // Might not be necesary
		return false;
	if (it->type == INDEX && container.back().index.empty())
		pushBackMultipleParams(container.back().index, getParam(*it));
	else if (it->type == ROOT && container.back().rroot.empty())
		container.back().rroot = getParam(*it);
	else if (it->type == METHOD && container.back().httpMethods.empty())
		pushBackMultipleParams(container.back().httpMethods, getParam(*it));
	else if (it->type == REDIRECT && container.back().redir.empty())
		container.back().redir = getParam(*it);
	else if (it->type == DIR_LISTING && container.back().dirListing == -1) {
		if (getParam(*it) == "on")
			container.back().dirListing = 1;
		else if (getParam(*it) == "off")
			container.back().dirListing = 0;
		else
			return false;
	}
	else
		return false;
	it++;
	return true;
}

// [parameters] ERROR_PAGES

template <>
bool Parser::parameterLstCase1<std::list<std::pair<int, std::string> > >(std::list<std::pair<int, std::string> >& container) {
	if (it == end)
		return false;
	std::cout << "\033[32m" <<  "Entered parameterLstCase1 ERROR_PAGE with: " << it->content << " | type: " << it->type << "\033[0m" << std::endl;
	int code = atoi(it->content.c_str());
	if (code == 0)
		return false;
	if (container.back().first == code)
		return false;	
	container.push_back(std::make_pair(code, getParam(*it)));
	it++;
	return true;
}

// [parameters] CGI

template <>
bool Parser::parameterLstCase1<std::list<std::pair<std::string, std::string> > >(std::list<std::pair<std::string, std::string> >& container) {
	if (it == end)
		return false;
	std::cout << "\033[32m" <<  "Entered parameterLstCase1 CGI with: " << it->content << " | type: " << it->type << "\033[0m" << std::endl;
	size_t i = it->content.find_first_of(':');
	if (i == std::string::npos)
		return false;
	std::string extension = it->content.substr(0, i);
	if (container.back().first == extension)
		return false;
	container.push_back(std::make_pair(extension, getParam(*it)));
	it++;
	return true;
}