#include "../inc/Client.hpp"
#include "../inc/Error.hpp"



Client::Client() : client_fd(0), addr_len(sizeof(client_addr)) {}
// { std::cout << "client constructor called" << std::endl; }

Client::~Client() { std::cout << "client destructor called" << std::endl; }

int	Client::acceptClient(int server_socket_fd, int epoll_fd)
{
	setServerFd(server_socket_fd);
	client_fd = accept(server_socket_fd, (sockaddr *)&client_addr, &addr_len);
	if (client_fd == -1)
		return ACCEPT_ERROR;
	event.data.fd = client_fd;
	//needs to check input, output and possible errors or hangups of clients at the same time
	event.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1)
		return EPOLL_ERROR;
	return 0;
}

void	Client::setServerFd(int fd)
{
	this->serverFd = fd;
}

int	Client::getClientFd() const
{
	return (client_fd);
}

int Client::getServerFd()
{
	return this->serverFd;
}