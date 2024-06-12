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
	Route *longestMatch = nullptr;
	size_t longestMatchLength = 0;
	for (auto &route: routes)
	{
		std::string routePath = route.getPath();
		if (directory_path.find(routePath) == 0 && routePath.length() > longestMatchLength)
		{
			longestMatch = &route;
			longestMatchLength = routePath.length();
		}
	}
	if (!longestMatch)
	{
		for (auto &route : routes)
		{
			if (route.getPath() == "/")
			{
				longestMatch = &route;
				break ;
			}
		}
	}
	return longestMatch;
}

bool Request::isDirectory(std::string path)
{
	struct stat sb;
	if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		return true;
	return false;
}

std::string Request::composePath(Route *route)
{
	std::string root = defineRoot(route);
	std::string index = defineIndex(route);
	if (isDirectory(root + request_map["Path"]))
	{
		if (root.back() != '/')
			root += "/";
		return root + request_map["Path"] + index;
	}
	return root + request_map["Path"];
}
//make a write request first and then check for availability to write to client_fd
void Request::HandleRequest()
{
	try
	{
		Route *route = matchRoute(request_map["Path"]);
		if (!route)
			throw NotFoundException();
		std::string path = composePath(route);
		std::cout << "Path: " << path << std::endl;


		std::cout << MAGENTA << "Handling a " << request_map["Method"] << " request!" << RESET << std::endl;
		// if (isCgiRequest(request_map["Path"]))
		// 	execute_cgi(request_map["Path"]);
		if (request_map["Method"] == "GET")
		{
			std::string response = Handle_GET(path);
			std::string response_header = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
			_serverInstance->create_write_request(response_header, client->getClientFd());
		}
		else if (request_map["Method"] == "POST")
		{
			std::string response = Handle_POST(path);
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

std::string Request::getErrorPath(const ServerException &e)
{
	if (dynamic_cast<const NotFoundException *>(&e))
		return config->matchErrorPage(404);
	else if (dynamic_cast<const BadRequestException *>(&e))
		return config->matchErrorPage(400);
	else if (dynamic_cast<const InternalServerErrorException *>(&e))
		return (config->matchErrorPage(500));
	else
		return ("<html><body><h1>Error page not found</h1></body></html>");

}
std::string Request::getErrorPage(const ServerException &e)
{
	std::string errorPagePath = getErrorPath(e);
	std::ifstream file(errorPagePath);
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
	if (route->getAlias().empty())
		root = config->getRoot();
	else
		root = route->getAlias();
	return root;
}
std::string Request::defineIndex(Route *route)
{
	std::string index;
	if (route->getIndex().empty())
		index = config->getIndex();
	else
		index = route->getIndex();
	return index;
}

std::string Request::normalizePath(std::string path)
{
	std::string normalizedPath = path;
	if (!normalizedPath.empty() && normalizedPath[0] == '/')
		normalizedPath.erase(0, 1);
	return normalizedPath;
}
std::string Request::Handle_GET(std::string path)
{
	std::ifstream file(path);
	if(!file.is_open() || !file.good())
		throw NotFoundException();
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}