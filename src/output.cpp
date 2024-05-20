#include "../inc/Server.hpp"
#include "../inc/Error.hpp"

int Server::exit_error(int exit_code, int line_num)
{
    if (exit_code == 1)
        std::cout << "Syntax error near line: " << line_num << std::endl;
    std::cout << RED << g_webserv_errors[exit_code] << RESET << std::endl;
    exit(exit_code);
}