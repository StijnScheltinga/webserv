#include "../inc/Server.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: ./webserv [config file]" << std::endl;
        return 1;
    }
    Server server(argv[1]);
    server.listen_to_socket();
    server.accept_connection();
    return 0;
}