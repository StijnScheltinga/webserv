#include "../inc/Parser.hpp"
#include "../inc/Server.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: ./webserv [config file]" << std::endl;
        return (1);
    }
	//pass config file path to parser
	Parser	parser(argv[1]);
    std::vector<Config> configVector = parser.getConfigVector();
	Server	server(configVector);

    return 0;
}