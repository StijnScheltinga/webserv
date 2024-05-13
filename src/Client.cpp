#include "../inc/Client.hpp"



Client::Client() : client_fd(0), addr_len(sizeof(client_addr)) { std::cout << "client constructor called" << std::endl; }

Client::~Client() { std::cout << "client destructor called" << std::endl; }

void	Client::acceptClient(int server_socket_fd, int epoll_fd)
{
	client_fd = accept(server_socket_fd, (sockaddr *)&client_addr, &addr_len);
	if (client_fd == -1)
		std::cout << "error client" << std::endl;
	event.data.fd = client_fd;
	event.events = EPOLLIN;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1)
		std::cout << "error client" << std::endl;
}

int	Client::getFd() const
{
	return (client_fd);
}