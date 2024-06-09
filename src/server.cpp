#include "../inc/Server.hpp"
#include "../inc/Request.hpp"
#include "../inc/Config.hpp"
#include <algorithm>
#include "../inc/Error.hpp"
#include <poll.h>
#include <vector>
#include <cstring>

int Server::listen_to_socket()
{
	int serverFd = socket(AF_INET, SOCK_STREAM, 0);
	int servernum = serverFds.size();
	if (serverFd == -1)
	{
		exitError(SOCK_FAIL);
	}
	std::memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = INADDR_ANY;
	std::cout << configs[servernum].getPort() << std::endl;
	sock_addr.sin_port = htons(configs[servernum].getPort());
	sock_addr_len = sizeof(sock_addr);
    if (bind(serverFd, (struct sockaddr *)&sock_addr, sock_addr_len) < 0)
    {
        close(serverFd);
        exitError(BIND_FAIL);
    }
    if (listen(serverFd, max_connections) < 0)
    {
        close(serverFd);
        exitError(LISTEN_FAIL);
    }
    std::ostringstream ss;
    ss <<  "Listening on address " << inet_ntoa(sock_addr.sin_addr) << " on port " << ntohs(sock_addr.sin_port);
    std::cout << GREEN << ss.str() << RESET << std::endl;
    return serverFd;
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
      remove_client(client_fd);
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

		Client	*client = getClientPtr(client_fd);
		if (!client)
			return ;
		Config	*config = getCorrectConfig(client);
		if (!config)
			return;

		Request(client, config, request_string.c_str(), this);
    }
}

//currently no checks for max clients
void	Server::add_client(int epoll_fd, int event_fd)
{
	Client	*client = new Client();

	//accepts client through accept function and adds it to the epoll instance
	int err = client->acceptClient(event_fd, epoll_fd);
	if (err == ACCEPT_ERROR)
	{
		delete client;
		std::cout << "accept error" << std::endl;
	}
	else if (err == EPOLL_ERROR)
	{
		close(client->getClientFd());
		delete client;
		std::cout << "epoll error" << std::endl;
	}
	else
		clientVec.push_back(client);
}

void	Server::remove_client(int client_fd)
{
	std::vector<Client*>::iterator it;
	for (it = clientVec.begin(); it != clientVec.end(); it++)
	{
		if (client_fd == (*it)->getClientFd())
		{
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
			close((*it)->getClientFd());
			delete (*it);
			clientVec.erase(it);
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
		exitError(EPOLL_ERROR);

	//only EPOLLIN for connecting with users
	for (size_t i = 0; i < serverFds.size(); i++)
	{
		struct epoll_event	event_server;
		event_server.events = EPOLLIN;
		event_server.data.fd = serverFds[i];
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverFds[i], &event_server) == -1)
			exitError(EPOLL_CTL_ERROR);
	}
	while (true)
	{
		struct epoll_event	events[MAX_EVENTS];
		int n_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (n_events == -1)
			exitError(EVENT_ERROR);

		for (int i = 0; i != n_events; i++)
		{
			int event_fd = events[i].data.fd;
			if (std::find(serverFds.begin(), serverFds.end(), event_fd) != serverFds.end())
				add_client(epoll_fd, event_fd);
			else if (events[i].events & (EPOLLHUP | EPOLLERR))
				remove_client(events[i].data.fd);
			else if (events[i].events & EPOLLIN)
				handle_request(events[i].data.fd);
			else if (events[i].events & EPOLLOUT)
				process_write_request(events[i].data.fd);
		}
	}
}

Client	*Server::getClientPtr(int clientFd)
{
	//get client object corresponding to client fd
	for (std::vector<Client*>::iterator it = clientVec.begin(); it != clientVec.end(); it++)
	{
		Client	*client	 = *it;
		if (client->getClientFd() == clientFd)
			return client;
	}
	return NULL;
}

Config *Server::getCorrectConfig(Client *client)
{
	for (std::vector<Config>::iterator it = configs.begin(); it != configs.end(); it++)
	{
		Config config_it = *it;
		if (config_it.getServerFd() == client->getServerFd())
			return (&(*it));
	}
	return NULL;
}

Server::Server(std::vector<Config> &configVector) : configs(configVector)
{
	std::cout << GREEN << "Starting Server..." << RESET << std::endl;

	for (size_t i = 0; i < configVector.size(); i++)
	{
		int serverFd = listen_to_socket();
		configVector[i].setServerFd(serverFd);
		serverFds.push_back(serverFd);
	}
	accept_connection();
}

Server::~Server()
{
    std::cout << RED << "Server closed" <<  RESET << std::endl;
}
