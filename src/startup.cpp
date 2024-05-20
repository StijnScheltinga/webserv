#include "../inc/Server.hpp"

void Server::TransferConfig()
{
    port = std::stoi(server_block.config_map["listen"][0]);
    server_name = server_block.config_map["ServerName"];
    directory_index = server_block.config_map["DirectoryIndex"][0];
    upload_dir = server_block.config_map["UploadDir"][0];
    max_client_body_size = std::stoi(server_block.config_map["MaxClientBody"][0]);
    cgi_extensions = server_block.config_map["CGIExtensions"];
    cgi_dir = server_block.config_map["CGIDir"][0];

    std::cout << std::endl;
}

void Server::init_server()
{   
    TransferConfig();

    max_connections = 3;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr_len = sizeof(sock_addr);
}

void Server::StartServer()
{
    init_server();
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd == -1)
    {
        exit_error(SOCK_FAIL, 0);
    }
}