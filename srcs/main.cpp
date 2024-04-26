/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ratavare <ratavare@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 12:28:28 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/25 17:19:16 by ratavare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <WebServer.hpp>
#include "Parser.hpp"

const char* RED_TEXT = "\033[31m";
const char* RESET_COLOR = "\033[0m";

int main(void)
{
	try
	{
		Lexer lex;
		lex.tokenize("config.yml");
		Lexer::printTokens(lex.getTokens());
		Parser parser;
		parser.parse(lex.getTokens());
		WebServer server;
		server.listen();
	}
	catch (const std::exception& e)
	{
		std::cout << RED_TEXT << "ERROR: " << e.what() << RESET_COLOR << std::endl;
		perror("");
	}
}

