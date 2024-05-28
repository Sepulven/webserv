#include <WebServer.hpp>
#include <Lexer.hpp>
#include <Parser.hpp>


const char* RED_TEXT = "\033[31m";
const char* RESET_COLOR = "\033[0m";

int main(int argc, char **argv)
{
	if (argc != 2)
		return (std::cout << "Invalid argument number" << std::endl, 1);
	Lexer *lex = new Lexer;
	Parser *par = new Parser;
	try
	{
		lex->tokenize(argv[1]);
		// Lexer::printTokens(lex->getTokens());   // * Print tokens.
		par->parse(lex->getTokens());
		// par->printServerNodes(par->getServerNodesIt());  // * Print server nodes.
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
