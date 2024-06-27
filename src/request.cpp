#include "../inc/Request.hpp"
#include "../inc/Config.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <fstream>
#include <sys/stat.h>

Request::Request(Client *client, Config *config, std::string requestString, Server *serverInstance) : _serverInstance(serverInstance), client(client), config(config), requestString(requestString)
{
	indexSearch = false;
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
	route = longestMatch;
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
	std::string alias = normalizePath(route->getAlias());
	std::string routePath = route->getPath();
	std::string requestPath = request_map["Path"];
	std::string path;
	if (alias.empty())
		path = normalizePath(config->getRoot()) + requestPath;
	else
	{
		if (routePath == "/")
			path = alias + requestPath;
		else
			path = alias + requestPath.substr(routePath.length());
	}
	if (isDirectory(path))
	{
		//if it is a directory we want the index file
		indexSearch = true;
		path.back() == '/' ? path += defineIndex(route) : path += "/" + defineIndex(route);
	}
	std::cout << path << std::endl;
	return path;
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
		if (isCgiRequest(path))
			execute_cgi(path);
		else if (request_map["Method"] == "GET")
		{
			std::string response = Handle_GET(path);
			std::string response_header = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
			_serverInstance->create_write_request(response_header, client->getClientFd());
		}
		else if (request_map["Method"] == "POST")
		{
			std::string response = Handle_POST(path, route);
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
		std::string response = getErrorPage(e);
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
	else if (dynamic_cast<const ForbiddenException *>(&e))
		return (config->matchErrorPage(403));
	else if (dynamic_cast<const InternalServerErrorException *>(&e))
		return (config->matchErrorPage(500));
	else
		return ("<html><body><h1>Error page not found</h1></body></html>");
}
std::string Request::getErrorPage(const ServerException &e)
{
	std::string errorPagePath = getErrorPath(e);
	std::cout << std::endl;
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
	std::stringstream ss;
	//specific to autoindex
	if (route && !file.is_open() && indexSearch && route->getAutoIndex())
		ss << createAutoIndex(path);
	else if (!file.is_open() && indexSearch)
		throw ForbiddenException();
	else if(!file.is_open() || !file.good())
		throw NotFoundException();
	else
		ss << file.rdbuf();
	return ss.str();
}

std::string Request::createAutoIndex(const std::string &path)
{
	std::cout << "autoindex called" << std::endl;
	std::stringstream autoIndex;
	autoIndex << "<html>\n<head><title>Index of /</title></head>";
	//get the directory
	std::string directory = path.substr(0, path.find_last_of('/'));
	std::cout << "path: " << path << std::endl;
	std::cout << "directory " << directory << std::endl;
	autoIndex << "<h1>Index of " << directory << "</h1><hr><ul>";
	try {
        for (const auto & entry : std::filesystem::directory_iterator(directory)) {
            autoIndex << "<li>" << entry.path() << "</li>" ;
        }
    } catch (const std::filesystem::filesystem_error& err) {
        std::cerr << "Filesystem error: " << err.what() << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
	autoIndex << "</ul><hr></body></html>";
	return autoIndex.str();
}