#include "../inc/Server.hpp"
#include "../inc/Request.hpp"
#include <poll.h>
#include <vector>

int Server::listen_to_socket()
{
    if (bind(server_socket_fd, (struct sockaddr *)&sock_addr, sock_addr_len) < 0)
    {
        close(server_socket_fd);
        exit_error(SOCK_FAIL, 0);
    }
    if (listen(server_socket_fd, max_connections) < 0)
    {
        close(server_socket_fd);
        exit_error(BIND_FAIL, 0);
    }
    std::ostringstream ss;
    ss << "Listening on address " << inet_ntoa(sock_addr.sin_addr) << " on port " << ntohs(sock_addr.sin_port);
    std::cout << ss.str() << std::endl;
    return 0;
}

void Server::handle_request(int client_fd)
{
    char buffer[1024] = {0};
    
    // In the future we will need to handle the case where the request is larger than 1024 bytes.\
    // maybe append to buffer until read returns 0.
    int valread = read(client_fd, buffer, 1024);
	std::cout << "valread: " << valread << std::endl;
    if (valread > 0)
    {
        buffer[valread] = '\0';
        Request request(client_fd, buffer, config_map);
        request.ParseRequest();
        request.HandleRequest();
    }
	else if (valread == 0)
	{
		//handle disconnect
		remove_client(client_fd);
	}
}

//currently no checks for max clients
void	Server::add_client(int epoll_fd)
{
	Client	*client = new Client();

	//accepts client through accept function and adds it to the epoll instance
	client->acceptClient(server_socket_fd, epoll_fd);

	//add client to client arr
	clientArr[clientIndex] = client;
	clientIndex++;
}

void	Server::remove_client(int client_fd)
{
	//loop through client array
	for (int i = 0; i <= clientIndex; i++)
	{
		//if fd of request to disconnect is same as client in arr, delete
		if (client_fd == clientArr[i]->getFd())
		{
			delete clientArr[i];
			std::cout << "client_deleted" << std::endl;
				clientIndex--;
		}
	}
}

/*create epoll instance
add server socket to epoll instance
accept client through server socket
add client to epoll instance*/
void	Server::accept_connection()
{
	int	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		exit_error(EPOLL_ERROR, 0);

	struct epoll_event	event_server;
	event_server.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR;
	event_server.data.fd = server_socket_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket_fd, &event_server) == -1)
		exit_error(EPOLL_CTL_ERROR, 0);
	while (true)
	{
		struct epoll_event	events[MAX_EVENTS];
		int n_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (n_events == -1)
			exit_error(EVENT_ERROR, 0);

		for (int i = 0; i != n_events; i++)
		{
			if (events[i].data.fd == server_socket_fd)
			{
				//handle new connection, create client
				add_client(epoll_fd);
			}
			else if (events[i].events & EPOLLIN)
			{
				//handle request
				std::cout << "handle request" << std::endl;
				handle_request(events[i].data.fd);
			}
			// else if (events[i].events & EPOLLOUT)
			// {
			// 	std::cout << "ready for writing" << std::endl;
			// }
			// else if (events[i].events & EPOLLERR)
			// {
			// 	std::cout << "error occured epoll" << std::endl;
			// }
			else if (events[i].events & EPOLLHUP)
			{
				std::cout << "user disconnected" << std::endl;
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

Server::Server(char *_config_file)
{
    config_file = _config_file;
    StartServer();
    std::cout << "Server started" << std::endl;
}

Server::~Server()
{
    close(server_socket_fd);
    std::cout << "Server destructor called" << std::endl;
}
