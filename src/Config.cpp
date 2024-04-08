#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

struct ConfigBlock
{
	std::vector<ConfigBlock> blocks;
	std::map<std::string, std::string> config_map;
};

ConfigBlock ParseConfig(std::string file)
{
	std::ifstream config_file(file);
	std::string line;
	ConfigBlock rootblock;
	ConfigBlock *curr_block = &rootblock;
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
				std::string block_name = line.substr(0, line.find('{') - 1);
				ConfigBlock new_block;
				curr_block->blocks.push_back(new_block);
				curr_block = &curr_block->blocks.back();
				curr_block->config_map["BlockName"] = block_name;
			}
			else if (line.find('}') != std::string::npos)
				curr_block = &rootblock;
			else
			{
				std::istringstream ss(line);
				std::string key, value;
				ss >> key;
				ss >> value;
				value.substr(value.find(';'));

				curr_block->config_map[key] = value;
			}
		}
	}
	else
	{
		std::cerr << "Failed to open config file" << std::endl;
	}
	return rootblock;
}

void PrintConfig(ConfigBlock *rootblock)
{
	std::cout << "Printing config" << std::endl;
	for (auto &block : rootblock->blocks)
	{
		std::cout << "Block: " << block.config_map["BlockName"] << std::endl;
		for (auto &config : block.config_map)
		{
			std::cout << config.first << " : " << config.second << std::endl;
		}
	}
}
