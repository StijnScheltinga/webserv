#include "../inc/Server.hpp"
#include "../inc/Request.hpp"
#include <poll.h>
#include <vector>

int Server::listen_to_socket()
{
    if (bind(socket_fd, (struct sockaddr *)&sock_addr, sock_addr_len) < 0)
    {
        close(socket_fd);
        return (exit_error("Failed to bind socket"));
    }
    if (listen(socket_fd, max_connections) < 0)
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
int Server::set_fds(fd_set *readfds, std::vector<int> client_sockets)
{
    int sd, max_sd = socket_fd;
    for (int i = 0; i < max_connections; i++)
    {
        sd = client_sockets[i];
        if (sd > 0)
            FD_SET(sd, readfds);
        if (sd > max_sd)
            max_sd = sd;
    }
    return max_sd;
}

void Server::add_socket_to_vec(int client_socket, std::vector<int> &client_sockets)
{
    for (int i = 0; i < max_connections; i++)
    {
        if (client_sockets[i] == 0)
        {
            client_sockets[i] = client_socket;
            return ;
        }
    }
}

int Server::accept_connection()
{
    fd_set readfds;
    int max_sd, client_socket;
    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    std::vector<int> client_sockets(max_connections, 0);
    while (true)
    {
        FD_ZERO(&readfds);
        FD_SET(socket_fd, &readfds);
        max_sd = set_fds(&readfds, client_sockets);
        if (select(max_sd + 1, &readfds, NULL, NULL, NULL) < 0)
            return (exit_error("Select failed"));
        if (FD_ISSET(socket_fd, &readfds))
        {
            client_socket = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
            if (client_socket < 0)
            {
                close(socket_fd);
                return (exit_error("Failed to accept connection"));
            }
            add_socket_to_vec(client_socket, client_sockets);
        }
        for (int i = 0; i < max_connections; i++)
        {
            if (FD_ISSET(client_sockets[i], &readfds))
                handle_request(client_sockets[i]);
        }
    }
    return 0;
}

Server::Server(char *config_file)
{
    if (StartServer(config_file))
        exit_error("Failed to start server");
    std::cout << "Server started" << std::endl;
}

Server::~Server()
{
    close(socket_fd);
    std::cout << "Server destructor called" << std::endl;
}
