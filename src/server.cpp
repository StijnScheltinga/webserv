#include "../inc/Server.hpp"

int Server::listen_to_socket()
{
    if (bind(socket_fd, (struct sockaddr *)&sock_addr, sock_addr_len) == -1)
        return (exit_error("Failed to bind socket"));
    if (listen(socket_fd, max_connections) == -1)
        return (exit_error("Failed to listen to socket"));
    std::ostringstream ss;
    ss << "Listening on address " << inet_ntoa(sock_addr.sin_addr) << " on port " << ntohs(sock_addr.sin_port);
    log(ss.str());
    return 0;
}

int Server::accept_connection()
{
    new_socket = accept(socket_fd, (struct sockaddr *)&sock_addr, &sock_addr_len);
    if (new_socket == -1)
        return (exit_error("Failed to accept connection"));
    return 0;
}

Server::Server()
{
    if (StartServer())
        exit_error("Failed to start server");
    std::cout << "Server started" << std::endl;
}

void Server::readRequest()
{
    char buffer[7000] = {0};
    int valread = read(new_socket, buffer, 7000);
    if (valread == -1)
        exit_error("Failed to read from socket");
    std::cout << buffer << std::endl;
}

void Server::sendResponse()
{
    std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\ngooffy gooffy\n";
    write (new_socket, response.c_str(), response.length());
}

Server::~Server()
{
    close(socket_fd);
    std::cout << "Server destructor called" << std::endl;
}
