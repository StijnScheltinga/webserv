#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "../inc/Config.hpp"

Config::Config()
{
	std::cout << "Config constructor called" << std::endl;
}

Config::~Config()
{
	std::cout << "Config destructor called" << std::endl;
}

void Config::ParseConfig(std::string file)
{
	std::ifstream config_file(file);
	std::string line;
	if (config_file.is_open())
	{
		while (std::getline(config_file, line))
		{
			line.erase(0, line.find_first_not_of("\t\r\n "));
			line.erase(line.find_last_not_of("\t\r\n ") + 1);
			if (line[0] == '#' || line.empty())
				continue;
			if (line.find('{') != std::string::npos)
			{
				//block.config_map["ServerName"][0] = line.substr(0, line.find('{') - 1);
				while (std::getline(config_file, line))
				{
					line.erase(0, line.find_first_not_of("\t\r\n "));
					line.erase(line.find_last_not_of("\t\r\n ") + 1);
					if (line[0] == '#' || line.empty())
						continue;
					if (line.find('}') != std::string::npos)
						break ;
					std::istringstream ss(line);
					std::string key, value;
					ss >> key;
					while (ss >> value)
						config_map[key].push_back(value);
					config_map[key].back().resize(config_map[key].back().size() - 1);
				}
				return ;
			}
		}
	}
	else
	{
		std::cerr << "Failed to open config file" << std::endl;
	}
	return ServerBlock();
}

// void PrintConfig(ServerBlock *rootblock)
// {
// 	std::cout << "Printing config" << std::endl;
// 	for (auto &block : rootblock->blocks)
// 	{
// 		std::cout << "Block: " << block.config_map["BlockName"] << std::endl;
// 		for (auto &config : block.config_map)
// 		{
// 			std::cout << config.first << " : " << config.second << std::endl;
// 		}
// 	}
// }
