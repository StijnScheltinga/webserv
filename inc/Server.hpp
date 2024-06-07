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
#include "Config.hpp"

class Server
{
    public:
        Server();
        ~Server();

        void TransferConfig();
        void parse_config();

        int listen_to_socket();
        void accept_connection();
		void add_client(int epoll_fd);
		void remove_client(int client_fd);
        void handle_request(int client_fd);
		void create_write_request(const std::string& data, int client_fd);
		void process_write_request(int client_fd);
        void readRequest();
        void sendResponse();

        // int set_fds(fd_set *set, std::vector<int> client_fds);
        // void    add_socket_to_vec(int client_fd, std::vector<int> &client_fds);

    private:
        int server_socket_fd;
        struct sockaddr_in sock_addr;
        socklen_t sock_addr_len;
        int port;
        int max_connections;
        unsigned int max_client_body_size;
        int	epoll_fd;

        std::vector<Client*> clientVec;

        typedef struct writeRequest {
          int 		fd;
          std::string	data;
        } writeRequest;

		std::vector<writeRequest*> writeRequests;

		const	Config*	config;

        std::map<std::string, std::string> config_map;
        std::string cgi_dir;
        std::string root;
        std::string ip;
        std::string directory_index;
        std::string upload_dir;
        std::vector<std::string> cgi_extensions;
        std::vector<std::string> server_name;
        
};


# define RESET      "\033[0m"
# define BLACK      "\033[30m"
# define RED        "\033[31m"
# define GREEN      "\033[32m"
# define YELLOW     "\033[33m"
# define BLUE       "\033[34m"
# define MAGENTA    "\033[35m"
# define CYAN       "\033[36m"
# define WHITE      "\033[37m"

#endif 