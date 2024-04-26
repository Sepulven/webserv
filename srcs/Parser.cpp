#include "Parser.hpp"

Parser::Parser() {}

Parser::~Parser() {}

void Parser::parse(std::list<token> lexerOutput) {
	tokens = lexerOutput;
}
