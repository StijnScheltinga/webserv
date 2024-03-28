#include "../inc/Server.hpp"

void Server::log(const std::string &message)
{
    std::cout << message << std::endl;
}

int Server::exit_error(const std::string &message)
{
    log("ERROR: " + message);
    exit(1);
}