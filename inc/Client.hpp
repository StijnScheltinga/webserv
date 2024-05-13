#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <netinet/in.h>
# include <sys/epoll.h>
# include <iostream>

class Client
{
	private:
		int					client_fd;
		struct sockaddr_in	client_addr;
		socklen_t 			addr_len;
		struct epoll_event	event;
	public:
		Client();
		~Client();
		void	acceptClient(int server_socket_fd, int epoll_fd);
		int		getFd() const;
};

#endif