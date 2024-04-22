#include "../inc/Server.hpp"

void Server::init_server()
{
    port = 8088;
    max_connections = 3;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr_len = sizeof(sock_addr);
	clientIndex = 0;
}

int Server::StartServer()
{
    init_server();
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd == -1)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }
    return 0;
}