#include "../inc/Server.hpp"

int main()
{
    Server server;
    server.listen_to_socket();
    server.accept_connection();
    return 0;
}