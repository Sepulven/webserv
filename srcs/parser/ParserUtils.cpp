#include "../../inc/parser/Parser.hpp"

void Parser::clearLastOperation(int type) {
	std::cout << "CLEARED VALUES ON TYPE " << type << std::endl;
	if (type == INDEX)
		serverNodes.back().index.pop_back();\
	if (type == LISTEN) {
		serverNodes.back().host = std::string();
		serverNodes.back().port = 0;
	}
	if (type == HOST)
		serverNodes.back().host = std::string();
	if (type == PORT)
		serverNodes.back().port = 0;
}

std::list<token>::iterator Parser::getLastTokenIt(std::list<token> tokens) {
	for (std::list<token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		last = it;
	}
	return last;
}