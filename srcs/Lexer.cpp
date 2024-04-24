#include "Lexer.hpp"

Lexer::Lexer() {}

Lexer::~Lexer() {}

void Lexer::printTokens(std::list<token> tokens) {
for (std::list<token>::iterator it = tokens.begin(); it != tokens.end(); it++)
	std::cout << it->content << "$ | ident level: " << it->identLevel << std::endl;
}

std::list<token> Lexer::getTokens(void) {return this->tokens;}

// Returns the level of identation on a line, returns -1 in case of bad identation.

int	Lexer::countIdent(std::string line) {
	unsigned int count = 0;
	for (std::string::iterator it = line.begin(); *it == ' '; it++)
		count++;
	if (count % 4 != 0)
		return -1;
		// throw Error("Bad identation in config file");
	return (count / 4);
}

std::string Lexer::trimLine(std::string line) {
	std::string::iterator it = line.end() - 1;
	while (*it != '#') {
		if (it == line.begin())
			return line;
		it--;
	}
	it--;
	size_t lastNonSpace = line.find_last_not_of(' ', it - line.begin());
	return (line.substr(0, lastNonSpace + 1));
}

token Lexer::new_token(std::string content, int identLevel) {
	token new_token;
	new_token.content = content;
	new_token.identLevel = identLevel;
	return new_token;
}

void Lexer::tokenize(std::string filePath) {
	std::ifstream inputFile(filePath.c_str());
	if (!inputFile)
		throw Error("Invalid config file path");
	std::string line;
	while (std::getline(inputFile, line)) {
		if (*line.begin() != '#' && !line.empty())
			tokens.push_back(new_token(trimLine(line), countIdent(line)));
	}
}
