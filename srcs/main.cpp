#include <WebServer.hpp>
#include <Lexer.hpp>
#include <Parser.hpp>

const char* RED_TEXT = "\033[31m";
const char* RESET_COLOR = "\033[0m";

int main(void)
{
	try
	{
		Lexer *lex = new Lexer;
		lex->tokenize("config.yml");
		Lexer::printTokens(lex->getTokens());   // Print tokens.
		Parser *par = new Parser;
		par->parse(lex->getTokens());
		delete lex;
		par->printServerNodes(par->getServerNodesIt());  // Print server nodes.
		WebServer server;
		server.listen();
		delete par;
	}
	catch (const std::exception& e)
	{
		std::cout << RED_TEXT << "ERROR: " << e.what() << RESET_COLOR << std::endl;
	}
}
