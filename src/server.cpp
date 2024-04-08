#include "../inc/Server.hpp"
#include "../inc/Request.hpp"
#include <poll.h>
#include <vector>

int Server::listen_to_socket()
{
    if (bind(server_socket_fd, (struct sockaddr *)&sock_addr, sock_addr_len) < 0)
    {
        close(server_socket_fd);
        return (exit_error("Failed to bind socket"));
    }
    if (listen(server_socket_fd, max_connections) < 0)
    {
        close(server_socket_fd);
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
	std::cout << valread << std::endl;
    if (valread > 0)
    {
        buffer[valread] = '\0';
        Request request(client_socket, buffer);
        request.ParseRequest();
        request.HandleRequest();
    }
}

void	Server::handle_new_connection(int epoll_fd)
{
	//accept new conection
	struct sockaddr_in	client_addr;
	socklen_t addr_len = sizeof(client_addr);
	int	client_fd = accept(server_socket_fd, (sockaddr *)&client_addr, &addr_len);
	if (client_fd == -1)
		exit_error("accept");
	//add it to eppol to check for read/write operations down the line
	struct epoll_event	event;
	event.events = EPOLLIN;
	event.data.fd = client_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1)
		exit_error("epoll_ctl");
}

void	Server::accept_connection()
{
	int	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		exit_error("failed to create epoll");

	struct epoll_event	event_server;
	event_server.events = EPOLLIN;
	event_server.data.fd = server_socket_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket_fd, &event_server) == -1)
		exit_error("eppol_ctl");
	while (true)
	{
		struct epoll_event	events[MAX_EVENTS];
		int n_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (n_events == -1)
			exit_error("error getting events");

		for (int i = 0; i != n_events; i++)
		{
			if (events[i].data.fd == server_socket_fd)
			{
				//handle new connection
				handle_new_connection(epoll_fd);
			}
			else
			{
				//handle request
				std::cout << "handle request" << std::endl;
				handle_request(events[i].data.fd);
			}
		}
	}
}

// int Server::set_fds(fd_set *readfds, std::vector<int> client_sockets)
// {
//     int sd = 0;
//     int max_sd = server_socket_fd;
//     for (int i = 0; i < max_connections; i++)
//     {
//         sd = client_sockets[i];
//         if (sd > 0)
//             FD_SET(sd, readfds);
//         if (sd > max_sd)
//             max_sd = sd;
//     }
//     return max_sd;
// }

// void Server::add_socket_to_vec(int client_socket, std::vector<int> &client_sockets)
// {
//     for (int i = 0; i < max_connections; i++)
//     {
//         if (client_sockets[i] == 0)
//         {
//             client_sockets[i] = client_socket;
//             return ;
//         }
//     }
// }


// int Server::accept_connection()
// {
//     fd_set readfds;
//     int max_sd;
//     int client_socket;
//     sockaddr_in client_addr;
//     socklen_t client_addr_len = sizeof(client_addr);
//     std::vector<int> client_sockets(max_connections, 0);
//     while (true)
//     {
//         FD_ZERO(&readfds);
//         FD_SET(server_socket_fd, &readfds);
//         max_sd = set_fds(&readfds, client_sockets);
//         if (select(max_sd + 1, &readfds, NULL, NULL, NULL) < 0)
//             return (exit_error("Select failed"));
//         if (FD_ISSET(server_socket_fd, &readfds))
//         {
//             client_socket = accept(server_socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
// 			std::cout << "lol" << std::endl;
//             if (client_socket < 0)
//             {
//                 close(server_socket_fd);
//                 return (exit_error("Failed to accept connection"));
//             }
//             add_socket_to_vec(client_socket, client_sockets);
//         }
//         for (int i = 0; i < max_connections; i++)
//         {
//             if (FD_ISSET(client_sockets[i], &readfds))
//                 handle_request(client_sockets[i]);
//         }
//     }
//     return 0;
// }

Server::Server()
{
    if (StartServer())
        exit_error("Failed to start server");
    std::cout << "Server started" << std::endl;
}

Server::~Server()
{
    close(server_socket_fd);
    std::cout << "Server destructor called" << std::endl;
}
