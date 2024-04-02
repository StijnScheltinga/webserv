#include "../inc/Server.hpp"
#include "../inc/Request.hpp"
#include <poll.h>
#include <vector>

int Server::listen_to_socket()
{
    if (bind(socket_fd, (struct sockaddr *)&sock_addr, sock_addr_len) == -1)
    {
        close(socket_fd);
		std::cout << strerror(errno) << std::endl;
        return (exit_error("Failed to bind socket"));
    }
    if (listen(socket_fd, max_connections) == -1)
    {
        close(socket_fd);
        return (exit_error("Failed to listen to socket"));
    }
    std::ostringstream ss;
    ss << "Listening on address " << inet_ntoa(sock_addr.sin_addr) << " on port " << ntohs(sock_addr.sin_port);
    log(ss.str());
    return 0;
}

void Server::handle_request(int client_socket)
{
    char buffer[1024] = {0};
    
    // In the future we will need to handle the case where the request is larger than 1024 bytes.\
    // maybe append to buffer until read returns 0.
    int valread = read(client_socket, buffer, 1024);
    if (valread > 0)
    {
        buffer[valread] = '\0';
        Request request(client_socket, buffer);
        request.ParseRequest();
        request.HandleRequest();
    }   
}
int Server::accept_connection()
{
    // We will use poll for concurrent i/o.
    // std::vector<pollfd> fds(max_connections + 1);
    // fds[0].fd = socket_fd;
    // fds[0].events = POLLIN;

    while (true)
    {
        sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0)
        {
            close(socket_fd);
            return (exit_error("Failed to accept connection"));
        }
        handle_request(client_socket);
        close (client_socket);
    }
    return 0;
}

Server::Server()
{
    if (StartServer())
        exit_error("Failed to start server");
    std::cout << "Server started" << std::endl;
}

Server::~Server()
{
    close(socket_fd);
    std::cout << "Server destructor called" << std::endl;
}
