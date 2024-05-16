#include "../inc/Server.hpp"

// void Server::parse_config()
// {
//     std::ifstream file(config_file);
//     if (!file.is_open())
//         (exit_error(OPEN_CONFIG_FAIL, 0));
//     std::string line;
//     int line_num = 0;
//     while(std::getline(file, line))
//     {
//         line_num++;
//         if (line[0] == '#' || line.empty())
//             continue ;
//         line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
//         line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
//         if (line.back() != ';')
//             exit_error(COLON_EOL, line_num);
//         std::istringstream ss(line);
//         std::string key, value;
//         ss >> key;
//         while (ss >> value)
//             config_map[key].push_back(value);
//         // remove colon
//         config_map[key].back().resize(config_map[key].back().size() - 1);
//     }
// }

void Server::TransferConfig()
{
    for (auto &config : server_block.config_map)
    {
        std::cout << config.first << ": ";
        for (auto &value : config.second)
        {
            std::cout << value << std::endl;
        }
    }
    port = std::stoi(server_block.config_map["listen"][0]);
    server_name = server_block.config_map["ServerName"];
    directory_index = server_block.config_map["DirectoryIndex"][0];
    root = server_block.config_map["root"][0];
    upload_dir = server_block.config_map["UploadDir"][0];
    max_client_body_size = std::stoi(server_block.config_map["MaxClientBody"][0]);
    cgi_extensions = server_block.config_map["CGIExtensions"];
    cgi_dir = server_block.config_map["CGIDir"][0];

    std::cout << std::endl;
}

void Server::init_server()
{   
    // parse_config();
    TransferConfig();

    max_connections = 3;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr_len = sizeof(sock_addr);
	clientIndex = 0;
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