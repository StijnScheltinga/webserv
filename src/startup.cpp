#include "../inc/Server.hpp"

void Server::parse_config()
{
    std::ifstream file(config_file);
    if (!file.is_open())
        (exit_error(OPEN_CONFIG_FAIL, 0));
    std::string line;
    int line_num = 0;
    while(std::getline(file, line))
    {
        line_num++;
        if (line[0] == '#' || line.empty())
            continue ;
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
        if (line.back() != ';')
            exit_error(COLON_EOL, line_num);
        std::istringstream ss(line);
        std::string key, value;
        ss >> key;
        while (ss >> value)
            config_map[key].push_back(value);
        // remove colon
        config_map[key].back().resize(config_map[key].back().size() - 1);
    }
}

void Server::TransferConfig()
{
    port = std::stoi(config_map["listen"][0]);
    server_name = config_map["server_name"];
    directory_index = config_map["DirectoryIndex"][0];
    root = config_map["root"][0];
    upload_dir = config_map["UploadDir"][0];
    max_client_body_size = std::stoi(config_map["MaxClientBody"][0]);
    cgi_extensions = config_map["CGIExtensions"];
    cgi_dir = config_map["CGIDir"][0];

    std::cout << std::endl;
}

void Server::init_server()
{   
    parse_config();
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
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
        exit_error(SOCK_FAIL, 0);
}