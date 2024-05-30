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
	// Server	server(parser.getConfigVector());

    // Server server(argv[1]);
    // server.listen_to_socket();
    // server.accept_connection();
    return 0;
}