#include "../inc/Server.hpp"
#include "../inc/Request.hpp"
#include "../inc/Config.hpp"
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
    ss <<  "Listening on address " << inet_ntoa(sock_addr.sin_addr) << " on port " << ntohs(sock_addr.sin_port);
    std::cout << GREEN << ss.str() << RESET << std::endl;
    return 0;
}

/*
creates an array of write requests.
will later be looked at if it is possible to write data to client
if so data from write request will be send to client and write request will be deleted
*/
void	Server::create_write_request(const std::string& data, int client_fd)
{
	writeRequest	*newRequest = new writeRequest;
	newRequest->fd = client_fd;
	newRequest->data = data;
	writeRequests.push_back(newRequest);
}

void	Server::process_write_request(int client_fd)
{
	if (writeRequests.empty())
		return ;
	std::vector<writeRequest*>::iterator it;
	for (it = writeRequests.begin(); it != writeRequests.end(); it++)
	{
		if (client_fd == (*it)->fd)
		{
			write((*it)->fd, (*it)->data.c_str(), (*it)->data.size());
			delete *it;
			writeRequests.erase(it);
			break ;
		}
	}
}

void Server::handle_request(int client_fd)
{
    char buffer[1024] = {0};
    std::string request_string;
    
    int valread = read(client_fd, buffer, 1024);
	if (valread == 0)
    {
      // handle disconnect
      remove_client(client_fd);
      std::cout << "disconnect" << std::endl;
    }
	else
	{
		while (valread > 0)
		{
			request_string.append(buffer, valread);
			if (valread < 1024)
				break ;
			valread = read(client_fd, buffer, 1024);
		}
		request_string.append("\0");
		Request request(client_fd, request_string.c_str(), server_block.config_map, this);
		request.ParseRequest();
		request.printMap();
		request.HandleRequest(request_string);
    }
}

//currently no checks for max clients
void	Server::add_client(int epoll_fd)
{
	Client	*client = new Client();

	//accepts client through accept function and adds it to the epoll instance
	int err = client->acceptClient(server_socket_fd, epoll_fd);
	if (err == ACCEPT_ERROR)
	{
		delete client;
		std::cout << "accept error" << std::endl;
	}
	else if (err == EPOLL_ERROR)
	{
		close(client->getFd());
		delete client;
		std::cout << "epoll error" << std::endl;
	}
	else
		clientVec.push_back(client);

	//add client to client vec
	// std::cout << "client accepted and put in vector of clients" << std::endl;
}

void	Server::remove_client(int client_fd)
{
	std::vector<Client*>::iterator it;
	//loop through client array
	for (it = clientVec.begin(); it != clientVec.end(); it++)
	{
		//if fd of request to disconnect is same as client in arr, delete
		if (client_fd == (*it)->getFd())
		{
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
			close((*it)->getFd());
			delete (*it);
			clientVec.erase(it);
			std::cout << "client_deleted from vector" << std::endl;
			break ;
		}
	}
}

/*create epoll instance
add server socket to epoll instance
accept client through server socket
add client to epoll instance*/
void	Server::accept_connection()
{
	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		exit_error(EPOLL_ERROR, 0);

	struct epoll_event	event_server;
	//only EPOLLIN for connecting with users
	event_server.events = EPOLLIN;
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
			else if (events[i].events & (EPOLLHUP | EPOLLERR))
			{
				//error or hangup, client needs to be dissconnected.
				std::cout << "hang up or error" << std::endl;
				remove_client(events[i].data.fd);
			}
			else if (events[i].events & EPOLLIN)
			{
				//handle request
				//std::cout << "handle request" << std::endl;
				handle_request(events[i].data.fd);
			}
			else if (events[i].events & EPOLLOUT)
			{
				//check for write request
				process_write_request(events[i].data.fd);
				// std::cout << "ready for writing" << std::endl;
			}
		}
	}
}

Server::Server(const char *config_file)
{
    server_block = ParseConfig(config_file);
	std::cout << GREEN << "Starting Server..." << RESET << std::endl;
    StartServer();
}

Server::~Server()
{
    close(server_socket_fd);
    std::cout << RED << "Server closed" <<  RESET << std::endl;
}
