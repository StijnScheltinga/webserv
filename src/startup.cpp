#include "../inc/Server.hpp"

std::map<std::string, std::vector<std::string>> Server::parse_config(char *filename)
{
    std::map<std::string, std::vector<std::string>> config_map;
    std::ifstream file(filename);
    if (!file.is_open())
        (exit_error("Failed to open configuration file"));
    std::string line;
    while(std::getline(file, line))
    {
        if (line[0] == '#' || line.empty())
            continue ;
        std::istringstream ss(line);
        std::string key, value;
        ss >> key;
        while (ss >> value)
        {
            if (value.find(';'))
            {
                config_map[key].push_back(value.substr(0, value.find(';')));
                break;
            }
            config_map[key].push_back(value);
        }
    }
    return config_map;
}

void Server::init_server(char *config_file)
{
    max_connections = 3;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr_len = sizeof(sock_addr);

    std::map<std::string, std::vector<std::string>> config_map = parse_config(config_file);
    port = std::stoi(config_map["listen"][0]);
    std::cout << "port: " << port << std::endl;
    server_name = config_map["server_name"];
    std::cout << "server_name: ";
    for (unsigned long i = 0; i < config_map["server_name"].size(); i++)
        std::cout << config_map["server_name"][i];
    std::cout << std::endl;
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