#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "../inc/Config.hpp"

Config::Config(const char *configFile)
{
	std::cout << "Config constructor called" << std::endl;
	ParseConfig(configFile);
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
			
		}
	}
	else
	{
		std::cerr << "Failed to open config file" << std::endl;
	}
}


void Config::printConfig() const
{
	std::cout << "printing config\n";
}
