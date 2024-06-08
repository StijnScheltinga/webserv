#include "../inc/Request.hpp"
#include "../inc/Config.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <fstream>
#include <sys/stat.h>

Request::Request(Client *client, Config *config, const char *requestString, Server *serverInstance) : _serverInstance(serverInstance), client(client), config(config), requestString(requestString)
{
	ParseRequest();
	HandleRequest();
}

Request::~Request() {}

void Request::ParseRequest()
{
	//First line always has the method, path and version.
	std::istringstream ss(requestString);
	std::string method, path, version;
	ss >> method >> path >> version;
	request_map["Method"] = method;
	request_map["Path"] = path;
	request_map["Version"] = version;

	std::string line;
	//skip the first line, it is parsed already.
	std::getline(ss, line);
	while (std::getline(ss, line))
	{
		size_t colon_pos = line.find(':');
		if (colon_pos != std::string::npos)
		{
			std::string key = line.substr(0, colon_pos);
			std::string value = line.substr(colon_pos + 2);
			request_map[key] = value;
		}
	}
}
Route *Request::matchRoute(std::string directory_path)
{
	std::vector<Route>&	routes = config->getRoutes();
	std::cout << "directory_path: " << directory_path << std::endl;
	std::string path = directory_path;
	for (std::vector<Route>::iterator it = routes.begin(); it != routes.end(); it++)
	{
		if (it->getPath() == path)
		{
			if (std::find(it->getAllowedMethods().begin(), it->getAllowedMethods().end(), request_map["Method"]) != it->getAllowedMethods().end())
				return &(*it);
		}
	}
	return nullptr;
}

bool Request::isDirectory(std::string path)
{
	struct stat sb;
	if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		return true;
	return false;

}

//make a write request first and then check for availability to write to client_fd
void Request::HandleRequest()
{
	try
	{
		std::string directory_path = request_map["Path"];
		if (!isDirectory(directory_path))
		{
			//cut down the path to the last slash to get the directory path.
			size_t last_slash = directory_path.find_last_of('/') + 1;
			directory_path = directory_path.substr(0, last_slash);
		}
		//check if the request_path corresponds to a location in the config file.
		Route *route = matchRoute(directory_path);
		if (!route)
			throw NotFoundException();
	
		std::cout << MAGENTA << "Handling a " << request_map["Method"] << " request!" << RESET << std::endl;
		// if (isCgiRequest(request_map["Path"]))
		// 	execute_cgi(request_map["Path"]);
		if (request_map["Method"] == "GET")
		{
			std::string response = Handle_GET(route);
			std::string response_header = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
			//create write request
			_serverInstance->create_write_request(response_header, client->getClientFd());
		}
		else if (request_map["Method"] == "POST")
		{
			std::string response = Handle_POST(route);
			std::string response_header = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
			_serverInstance->create_write_request(response, client->getClientFd());
		}
		// else if (request_map["Method"] == "DELETE")
		// {
		// 	Handle_DELETE();
		// }
	}
	catch (const ServerException &e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
		std::string response = getErrorPage(BadRequestException());
		std::string response_header = BAD_REQUEST + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
		_serverInstance->create_write_request(response_header, client->getClientFd());
	}

}

void	Request::printMap(void)
{
	std::map<std::string, std::string>::iterator	it = request_map.begin();

	std::cout << "---request---\n";
	for (;it != request_map.end(); it++)
		std::cout << "key: " << it->first << ", value: " << it->second << "\n";
	std::cout << "---end request---" << std::endl;
}

std::string Request::getErrorPage(const ServerException &e)
{
	std::string path = config->getRoot();
	if (e.what() == "404 Page not found")
	{
		for (std::vector<ErrorPage>::iterator it = config->getErrorPages().begin(); it != config->getErrorPages().end(); it++)
		{
			for (std::vector<int>::iterator it2 = it->getStatusCodesVector().begin(); it2 != it->getStatusCodesVector().end(); it2++)
			{
				if (*it2 == 404)
				{
					std::cout << "404 error, path: " << it->getPath() << std::endl;
					path += it->getPath();
				}
			}
		}
	}
	else if (e.what() == "400 Bad request")
	{
		for (std::vector<ErrorPage>::iterator it = config->getErrorPages().begin(); it != config->getErrorPages().end(); it++)
		{
			for (std::vector<int>::iterator it2 = it->getStatusCodesVector().begin(); it2 != it->getStatusCodesVector().end(); it2++)
			{
				if (*it2 == 400)
					path += it->getPath();
			}
		}
	}
	else if (e.what() == "500 Internal server error")
	{
		for (std::vector<ErrorPage>::iterator it = config->getErrorPages().begin(); it != config->getErrorPages().end(); it++)
		{
			for (std::vector<int>::iterator it2 = it->getStatusCodesVector().begin(); it2 != it->getStatusCodesVector().end(); it2++)
			{
				if (*it2 == 500)
					path += it->getPath();
			}
		}
	}
	std::ifstream file(path);
	if (!file.is_open() || !file.good())
	{
		std::cerr << "Error page not found" << std::endl;
		exit(1);
	}
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

std::string Request::defineRoot(Route *route)
{
	std::string root;
	if (route->getRoot().empty())
		root = config->getRoot();
	else
		root = route->getRoot();
	return root;
}
std::string Request::defineIndex(Route *route)
{
	std::string index;
	if (route->getIndex().empty() && route->getAutoIndex())
		index = "index.html";
	else if (route->getIndex().empty() && !route->getAutoIndex())
		throw NotFoundException();
	else
		index = route->getIndex();
	return index;
}
std::string Request::Handle_GET(Route *route)
{
	std::string path;
	std::string root = defineRoot(route);
	std::string index = defineIndex(route);
	if (!index.empty() && request_map["Path"].back() == '/')
		path = root + request_map["Path"] + "/" + index;
	else
		path = root + request_map["Path"];
	std::cout << "path: " << path << std::endl;
	std::ifstream file(path);
	if(!file.is_open() || !file.good())
		throw NotFoundException();
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}