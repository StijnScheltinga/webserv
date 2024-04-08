#include "../inc/Server.hpp"

std::map<std::string, std::string> Server::parse_config(char *filename)
{
    std::map<std::string, std::string> config_map;
    std::ifstream file(filename);
    // if (!file.open())
    //     (exit_error("Failed to open configuration file"));
    std::string line;
    while(std::getline(file, line))
    {
        if (line[0] == '#' || line.empty())
            continue ;
        std::istringstream ss(line);
        std::string key, value;
        ss >> key >> value;
        config_map[key] = value;
    }
    return config_map;
}

void Server::init_server(char *config_file)
{
    std::map<std::string, std::string> config_map = parse_config(config_file);
    port = std::stoi(config_map["listen"]);
    max_connections = 3;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr_len = sizeof(sock_addr);
}

int Server::StartServer(char *config_file)
{
    init_server(config_file);
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }
    return 0;
}