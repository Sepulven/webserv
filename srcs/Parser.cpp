#include "Parser.hpp"

Parser::Parser() {}

Parser::~Parser() {}

bool Parser::checkIndent(token token) {
	if (token.identLevel < 0)
		return false;
	return true;
}

void Parser::parse(std::list<token> tokens) {
	for (std::list<token>::iterator it = tokens.begin();
	it != tokens.end(); it++) {
		if (!checkIndent(*it))
			throw Lexer::Error("Bad identation in config file.");
	}
}
