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
