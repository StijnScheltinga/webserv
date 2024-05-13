#ifndef SERVER_HPP
# define SERVER_HPP

# define MAX_EVENTS 64
# define MAX_CLIENTS 100

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
#include "Client.hpp"
#include <fstream>
#include <map>
#include "Error.hpp"

class Server
{
    public:
        Server();
        Server(char *config);
        ~Server();

        void init_server();
        void TransferConfig();
        void parse_config();
        void StartServer();

        int listen_to_socket();
        void accept_connection();
		void add_client(int epoll_fd);
		void remove_client(int client_fd);
        void handle_request(int client_fd);
        void readRequest();
        void sendResponse();
        
        int exit_error(int exit_code, int line_num);

        // int set_fds(fd_set *set, std::vector<int> client_sockets);
        // void    add_socket_to_vec(int client_socket, std::vector<int> &client_sockets);

    private:
        int server_socket_fd;

		Client *clientArr[MAX_CLIENTS];
		int	clientIndex;
        struct sockaddr_in sock_addr;
        socklen_t sock_addr_len;

        int port;
        int max_connections;
        unsigned int max_client_body_size;

        const char *config_file;
        std::map<std::string, std::vector<std::string> > config_map;
        std::string cgi_dir;
        std::string root;
        std::string ip;
        std::string directory_index;
        std::string upload_dir;
        std::vector<std::string> cgi_extensions;
        std::vector<std::string> server_name;
        
};

#endif 