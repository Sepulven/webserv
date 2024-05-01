#include "../../inc/parser/Parser.hpp"

Parser::Parser() {}

Parser::~Parser() {}

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

	// token ola;
	// ola.content = "route:";
	// std::cout << getParam(ola) << "$" << std::endl;

	configuration();

	// // For debugging purposes.
	// for (std::list<t_server>::iterator it = serverNodes.begin(); it != serverNodes.end(); it++)
	// 	std::cout << "SERVER NODE DETECTED!" << std::endl;
}

// Start of BNF syntax validator and 'serverNodes' composer based on syntax.txt.

// Main <configuration> function.

bool Parser::configuration() {
	std::list<token>::iterator tmp = it;
	if (configurationCase1()) // Missing new serverNode allocator on case functions.
		return true;
	it = tmp;
	if (configurationCase2())
		return true;
	return false;
}

// <server_block> <configuration>

bool Parser::configurationCase1() {
	if (it->type != SERVERBLOCK || it->identLevel != 0)
		return false;
	it++;
	t_server newServerNode;
	serverNodes.push_back(newServerNode);
	if (!serverBlock()) {
		serverNodes.pop_back();
		return false;
	}
	configuration(); // Not checking return outputs may cause issues.
	return true;
}

// <server_block>

bool Parser::configurationCase2() {
	if (it->type != SERVERBLOCK || it->identLevel != 0)
		return false;
	it++;
	t_server newServerNode;
	serverNodes.push_back(newServerNode);
	if (!serverBlock()) {
		serverNodes.pop_back();
		return false;
	}
	if (it != end)
		return false;
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

// <block_dirs> [path] <directives>

bool Parser::directivesCase1() {
	if (it->type != ROUTE)
		return false;
	std::string routePath = getRoute(*it);
	if (routePath.empty())
		return false;
	t_route newRoute;
	serverNodes.back().route.push_back(newRoute);
	serverNodes.back().route.back().path = routePath;
	it++;
	if (!blockDirs()) { // Might need specific function to acess t_route structure.
		serverNodes.back().route.pop_back();
		return false;
	}
	directives();
	return true;
}

// <block_dirs> <directives>

bool Parser::directivesCase2() {
	if (it->type != LISTEN || it->type != ERROR_PAGE_BLOCK)
		return false;
	int flag = it->type == LISTEN ? LISTEN : ERROR_PAGE_BLOCK;
	it++;
	if (!blockDirs())
		return false;
	directives();
	return true;
}

// <simple_dir> <directives>

bool Parser::directivesCase3() {
	if (it->type != NAME || it->type != ROOT || it->type != INDEX || 
	it->type != MAX_CBSIZE || it->type != MAX_CONN)
		return false;
	if (!parameterLst(0));
		return false;
	directives();
	return true;
}

// <block_dirs> [path] *

bool Parser::directivesCase4() {
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
	if (it->type != LISTEN || it->type != ERROR_PAGE_BLOCK)
		return false;
	int flag = it->type == LISTEN ? LISTEN : ERROR_PAGE_BLOCK;
	it++;
	if (!blockDirs(flag))
		return false;
	return true;
}

// <simple_dir>

bool Parser::directivesCase6() {
	if (it->type != NAME || it->type != ROOT || it->type != INDEX || 
	it->type != MAX_CBSIZE || it->type != MAX_CONN)
		return false;
	it++;
	if (!parameterLst(0));
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
	if (it->identLevel != 2)
		return false;
	if (flag == ROUTE)
		if (!parameterLst(serverNodes.back().route.back()))
			return false;
	if (flag == LISTEN)
		if (!parameterLst(0))
			return false;
	if (flag == ERROR_PAGE_BLOCK)
		if (!parameterLst(serverNodes.back().errorPages))
			return false;
	it++;
	return true;
}

// CHECK ITERATION, TOKENIZATION NOT IN [name]->[param] FORM.
// ACTUAL TOKENIZATION: [name][param]->[name][param].