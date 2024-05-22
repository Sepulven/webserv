#include "Lexer.hpp"

Lexer::Lexer() {
	std::string tmp[] = {"server", "listen", "host", "port", "server_name",
	"root", "index", "error_pages", "max_cbsize", "max_conn", "route", "http_methods",
	"dir_listing", "redirect", "cgi"};
	for (int i = 0; i < 15; i++)
		types.insert(std::make_pair(tmp[i], i));
}

Lexer::~Lexer() {}

void Lexer::printTokens(std::list<token> tokens) {
	std::cout << "============Lexer============" << std::endl << std::endl;
	for (std::list<token>::iterator it = tokens.begin(); it != tokens.end(); it++)
		std::cout << it->content << " | type: " << it->type 
		<< " | ident level: " << it->identLevel << std::endl;
	std::cout << std::endl;
}

std::list<token> Lexer::getTokens(void) {return this->tokens;}

// Returns the level of identation on a line, returns -1 in case of bad identation.

int	Lexer::countIdent(std::string line) {
	unsigned int count = 0;
	for (std::string::iterator it = line.begin(); *it == ' '; it++)
		count++;
	if (count % 4 != 0)
		return -1;
	return (count / 4);
}

bool Lexer::isspace(std::string str) {
	for (std::string::iterator it = str.begin(); it != str.end(); it++) {
		if (!std::isspace(*it))
			return false;
	}
	return true;
}

std::string Lexer::trimComments(std::string line) {
	if (line.empty() || isspace(line) || *line.begin() == '#')
		return std::string();
	size_t i = line.find_first_of('#');
	if (isspace(line.substr(0, i)))
		return std::string();
	std::string::iterator it = line.end() - 1;
	while (*it != '#') {
		if (it == line.begin())
			return line;
		it--;
	}
	it--;
	size_t lastNonSpace = line.find_last_not_of(' ', it - line.begin());
	return line.substr(0, lastNonSpace + 1);
}

void Lexer::trimIdent(std::string& content) {
	size_t i = content.find_first_not_of(' ');
	content = content.substr(i);
}

int Lexer::identifyToken(token& token) {
	if (!std::strncmp(token.content.c_str(), "route ", 6))
		return 10;
	if (token.content[0] == '.' && token.identLevel == 2)
		return 15;
	size_t i = token.content.find_first_of(":");
	std::string tmp = token.content.substr(0, i);
	for (std::map<std::string, int>::iterator it = types.begin();
	it != types.end(); it++) {
		if (!std::strcmp(tmp.c_str(), it->first.c_str()))
			return it->second;
	}
	return -1;
}

token Lexer::newToken(std::string content, int identLevel) {
	token newToken;
	newToken.content = content;
	newToken.identLevel = identLevel;
	return newToken;
}

void Lexer::tokenize(std::string filePath) {
	std::ifstream inputFile(filePath.c_str());
	if (!inputFile)
		throw Error("Invalid config file path");
	std::string line;
	while (std::getline(inputFile, line)) {
		std::string newLine = trimComments(line);
		if (!newLine.empty()) {
			tokens.push_back(newToken(newLine, countIdent(newLine)));
			trimIdent((tokens.back().content));
			tokens.back().type = identifyToken(tokens.back());
		}
	}
}
