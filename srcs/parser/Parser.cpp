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

void Parser::parse(std::list<token> tokens) {
	for (std::list<token>::iterator it = tokens.begin();
	it != tokens.end(); it++) {
		if (!checkIndent(*it))
			throw Lexer::Error("Bad identation in config file.");
	}
	this->it = tokens.begin();
	configuration();

	// For debugging purposes.
	for (std::list<t_server>::iterator it = serverNodes.begin(); it != serverNodes.end(); it++)
		std::cout << "SERVER NODE DETECTED!" << std::endl;
}

// Start of BNF syntax validator and 'serverNodes' composer based on syntax.txt.

// Main <configuration> function.

bool Parser::configuration() {
	// std::list<token>::iterator tmp = it;
	t_server node;
	serverNodes.push_back(node);
	configurationCase1();
	return true;
}

// <server_block> <configuration>

bool Parser::configurationCase1() {
	if (it->type != SERVERBLOCK)
		return false;
	it++;
	serverBlock
	if (it->type != SERVERBLOCK)
		return false;
	configuration();
	return true;
}
