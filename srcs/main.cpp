#include <WebServer.hpp>
#include <Lexer.hpp>
#include <Parser.hpp>


const char* RED_TEXT = "\033[31m";
const char* RESET_COLOR = "\033[0m";

int main(void)
{
	Lexer *lex = new Lexer;
	Parser *par = new Parser;
	try
	{
		lex->tokenize("config.yml");
		Lexer::printTokens(lex->getTokens());   // * Print tokens.
		par->parse(lex->getTokens());
		par->printServerNodes(par->getServerNodesIt());  // * Print server nodes.
		WebServer server(par->getServerNodes());
		server.listen();
	}
	catch (const std::exception& e)
	{
		std::cout << RED_TEXT << "ERROR: " << e.what() << RESET_COLOR << std::endl;
	}
	delete lex;
	delete par;
}
