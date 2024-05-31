#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "../inc/Config.hpp"
#include "../inc/Route.hpp"
#include "../inc/ErrorPage.hpp"

Config::Config(std::vector<std::string> serverBlock) : serverBlock(serverBlock)
{
	std::cout << "Config constructor called" << std::endl;
	ParseConfig();
}

Config::~Config()
{
	std::cout << "Config destructor called" << std::endl;
}

void Config::addRoute(std::string route)
{
	// Route newRoute(route);
	// routes.push_back(newRoute);
}

void Config::ParseConfig()
{
	for (std::vector<std::string>::iterator it = serverBlock.begin(); it != serverBlock.end(); it++)
	{
		std::string line = *it;
		if (line.find("location") == 0)
		{
			addRoute(line);
			continue ;
		}
		std::string key, value;
		std::istringstream iss(line);
		iss >> key;
		value = line.substr(key.length());
		value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));
		if (configHandlers.find(key) != configHandlers.end())
			(this->*configHandlers[key])(value);
	}
}


void Config::setServerName(std::string serverName)
{
	std::istringstream	ss(serverName);
	std::string 		token;
	while (std::getline(ss, token, ' '))
	{
		server_name.push_back(token);
	}
	for (std::vector<std::string>::iterator it = server_name.begin(); it != server_name.end(); it++)
	{
		std::cout << "Server name: " << *it << std::endl;
	}
}

void Config::setPort(std::string port)
{
	size_t length = port.length();
	if (length == 0 || length > 5)
	{
		std::cerr << "Port number must be between 1 and 5 digits" << std::endl;
		exit(1);
	}
	for (size_t i = 0; i < length; i++)
	{
		if (!std::isdigit(port[i]))
		{
			std::cerr << "Port number must be a number" << std::endl;
			exit(1);
		}
	}
	this->port = std::stoi(port);
	if (this->port < 1 || this->port > 65535)
	{
		std::cerr << "Port number must be between 1 and 65535" << std::endl;
		exit(1);
	}
	std::cout << "Port: " << this->port << std::endl;
}

char Config::getMaxClientBodySizeSuffix(std::string &clientMaxBodySize)
{
	char suffix;
	if (clientMaxBodySize.end()[-1] == 'k' || clientMaxBodySize.end()[-1] == 'K')
	{
		suffix = 'k';
		clientMaxBodySize.pop_back();
	}
	else if (clientMaxBodySize.end()[-1] == 'm' || clientMaxBodySize.end()[-1] == 'M')
	{
		suffix = 'm';
		clientMaxBodySize.pop_back();
	}
	else if (clientMaxBodySize.end()[-1] == 'g' || clientMaxBodySize.end()[-1] == 'G')
	{
		suffix = 'g';
		clientMaxBodySize.pop_back();
	}
	else
		suffix = 'b';
	return suffix;
}

void Config::setClientMaxBodySize(std::string clientMaxBodySize)
{
	char suffix = getMaxClientBodySizeSuffix(clientMaxBodySize);
	size_t length = clientMaxBodySize.length();
	if (length == 0)
	{
		std::cerr << "Please provide a number for client_max_body_size" << std::endl;
		exit(1);
	}
	for (size_t i = 0; i < length; i++)
	{
		if (!std::isdigit(clientMaxBodySize[i]))
		{
			std::cerr << "client_max_body_size must be a number" << std::endl;
			exit(1);
		}
	}
	if (suffix == 'k')
		this->client_max_body_size = std::stoi(clientMaxBodySize) * 1024;
	else if (suffix == 'm')
		this->client_max_body_size = std::stoi(clientMaxBodySize) * 1024 * 1024;
	else if (suffix == 'g')
		this->client_max_body_size = std::stoi(clientMaxBodySize) * 1024 * 1024 * 1024;
	else
		this->client_max_body_size = std::stoi(clientMaxBodySize);

	std::cout << "Client max body size: " << this->client_max_body_size << std::endl;
}

void Config::setErrorPage(std::string errorPage)
{
	std::istringstream	ss(errorPage);
	std::string 		token;
	ErrorPage			errorPageObj(errorPage);
	error_pages.push_back(errorPageObj);
	for (std::vector<ErrorPage>::iterator it = error_pages.begin(); it != error_pages.end(); it++)
	{
		std::cout << "Error page path: " << it->getPath() << std::endl;
		std::vector<int> statusCodes = it->getStatusCodesVector();
		for (std::vector<int>::iterator it = statusCodes.begin(); it != statusCodes.end(); it++)
		{
			std::cout << "Error page status code: " << *it << std::endl;
		}
	}
}

void Config::setRoot(std::string root)
{
	this->root = root;
	std::cout << "Root: " << this->root << std::endl;
}


void Config::printConfig() const
{
}
