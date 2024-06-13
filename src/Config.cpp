#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include "../inc/Config.hpp"
#include "../inc/Route.hpp"
#include "../inc/ErrorPage.hpp"

Config::Config(std::vector<std::string> serverBlock) : serverBlock(serverBlock)
{
	ParseConfig();
}

Config::~Config()
{
}

void Config::addRoute(std::vector<std::string>::iterator &it, std::vector<std::string>::const_iterator &end)
{
	Route newRoute(it, end);
	routes.push_back(newRoute);
}

void Config::ParseConfig()
{
	for (std::vector<std::string>::iterator it = serverBlock.begin(); it != serverBlock.end(); it++)
	{
		std::string line = *it;
		if (line.find("location") == 0)
		{
			std::vector<std::string>::const_iterator endline = serverBlock.end();
			addRoute(it, endline);
			continue ;
		}
		std::string key, value;
		std::istringstream iss(line);
		iss >> key;
		value = line.substr(key.length());
		value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));
		if (configHandlers.find(key) != configHandlers.end())
			(this->*configHandlers[key])(value);
		else
		{
			std::cout << "Unknown directive: \"" << key << "\" in server block" << std::endl;
			exit(1);
		}
	}
	printConfig();
}


void Config::setServerName(std::string serverName)
{
	std::istringstream	ss(serverName);
	std::string 		token;
	while (std::getline(ss, token, ' '))
		server_name.push_back(token);
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
}

void Config::setErrorPage(std::string errorPage)
{
	std::istringstream	ss(errorPage);
	ErrorPage			errorPageObj(errorPage);
	error_pages.push_back(errorPageObj);
}

void Config::setRoot(std::string root)
{
	this->root = root;
}

void Config::setServerFd(int fd)
{
	this->serverFd = fd;
}

void Config::setIndex(std::string index)
{
	this->index = index;
}

int Config::getPort() const
{
	return port;
}

int Config::getServerFd()
{
	return this->serverFd;
}

std::string	Config::getRoot() const
{
	return this->root;
}

std::vector<ErrorPage> Config::getErrorPages() const
{
	return error_pages;
}

std::string Config::matchErrorPage(int statusCode)
{
    for (size_t i = 0; i < error_pages.size(); i++)
    {
        std::vector<int> statusCodes = error_pages[i].getStatusCodesVector();
        if (std::find(statusCodes.begin(), statusCodes.end(), statusCode) != statusCodes.end())
            return error_pages[i].getPath();
    }
    return "";
}

std::vector<Route> &Config::getRoutes()
{
	return routes;
}
std::string Config::getIndex()
{
	return index;
}

void Config::printConfig() const
{
	std::cout << "server_name: ";
	for (size_t i = 0; i < server_name.size(); i++)
		std::cout << server_name[i] << " ";
	std::cout << std::endl;
	std::cout << "port: " << port << std::endl;
	std::cout << "client_max_body_size: " << client_max_body_size << std::endl;
	std::cout << "root: " << root << std::endl;
	std::cout << "error_pages: " << std::endl;
	for (size_t i = 0; i < error_pages.size(); i++)
		error_pages[i].printErrorPage();
	std::cout << "routes: " << std::endl;
	for (size_t i = 0; i < routes.size(); i++)
		routes[i].printRoute();
	std::cout << std::endl;
}