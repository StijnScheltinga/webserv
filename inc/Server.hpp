#ifndef SERVER_HPP
# define SERVER_HPP

# define MAX_EVENTS 64

#include <iostream>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <vector>
#include <future>
#include <sys/epoll.h>

class Server
{
    public:
        Server();
        ~Server();

        void init_server();
        int StartServer();

        int listen_to_socket();
        void accept_connection();
		void handle_new_connection(int epoll_fd);
        void handle_request(int client_socket);
        void readRequest();
        void sendResponse();
        
        void log(const std::string &message);
        int exit_error(const std::string &message);

        // int set_fds(fd_set *set, std::vector<int> client_sockets);
        // void    add_socket_to_vec(int client_socket, std::vector<int> &client_sockets);

    private:
        std::string ip;
        int server_socket_fd;
        int port;
        int max_connections;
        struct sockaddr_in sock_addr;
        socklen_t sock_addr_len;
        struct epoll_event	events[MAX_EVENTS];
};

#endif 