#pragma once

#include <iostream>
#include <algorithm>
#include <list>
#include <vector>
#include <fstream>
#include <sstream>

enum nodeType {
	SERVBLOCK,
	LISTEN,
	HOST,
	PORT,
	NAME,
	ROOT,
	INDEX,
	ERROR_PAGE_BLOCK,
	ERROR_NAME,
	MAX_CBSIZE,
	MAX_CONN,
	ROUTE,
	METHOD,
	DIR_LISTING
};

typedef struct token {
	std::string content;
	int			type;
	int			identLevel;
} token;


class Lexer
{
private:
	std::list<token> tokens;
	std::vector<std::string> types;
	std::string		 trimComments(std::string line);
	void			 trimIdent(std::string& content);
	void			 identifyToken(token& token);
	int				 countIdent(std::string line);
	token			 newToken(std::string content, int identLevel);

public:
					 Lexer();
					 ~Lexer();

	void			 tokenize(std::string filePath);
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
