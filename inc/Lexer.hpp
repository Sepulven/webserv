#pragma once

#include <iostream>
#include <algorithm>
#include <list>
#include <fstream>
#include <sstream>

typedef struct token {
	std::string content;
	int			identLevel;
} token;


class Lexer
{
private:
	std::list<token> tokens;
public:
				Lexer();
				~Lexer();

	void		tokenize(std::string filePath);

	int			countIdent(std::string line);
	std::string	trimLine(std::string line);
	token		newToken(std::string content, int identLevel);


	static void 	 printTokens(std::list<token> tokens);
	std::list<token> getTokens(void);

	class Error : public std::exception
	{
	private:
		const char *msg;
		virtual const char * what() const throw() {
			return msg;
		}
	public:
		Error(const char* _msg) : msg(_msg) {}
	};
};
