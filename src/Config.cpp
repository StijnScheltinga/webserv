#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "../inc/Config.hpp"

Config::Config(std::vector<std::string> serverBlock) : serverBlock(serverBlock)
{
	std::cout << "Config constructor called" << std::endl;
	ParseConfig();
}

Config::~Config()
{
	std::cout << "Config destructor called" << std::endl;
}

void Config::ParseConfig()
{

}


void Config::printConfig() const
{
	std::cout << "printing config\n";
}
