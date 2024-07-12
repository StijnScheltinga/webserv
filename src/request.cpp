#include "../inc/Request.hpp"
#include "../inc/Config.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <fstream>
#include <sys/stat.h>

Request::Request(Client *client, std::string requestString, Server *serverInstance) : _serverInstance(serverInstance), client(client), requestString(requestString)
{
	ParseRequest();
	printMap();
	ChooseServerConfig();
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
    while (std::getline(ss, line)) 
	{
        size_t delimiter_pos = line.find(':');
        if (delimiter_pos != std::string::npos) 
		{
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 2);
			value.erase(value.find_last_not_of(" \t\r\n") + 1);
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
	return path;
}

void	Request::ChooseServerConfig()
{
	//returns a reference to config vector
	std::vector<Config>	&configs = _serverInstance->getConfigs();
	std::vector<Config*> possibleConfigs;

	//first get configs for port and host of request
	for (std::vector<Config>::iterator it = configs.begin(); it != configs.end(); it++)
	{
		//if port and host match from client and config, save config in posibble config vector
		std::cout << "config server fd: " << (*it).getServerFd() << ", client server fd: " << client->getServerFd() << std::endl;  
		if ((*it).getServerFd() == client->getServerFd())
			possibleConfigs.push_back(&(*it));
	}
	//then look for config where host == server name
	for (std::vector<Config*>::iterator it = possibleConfigs.begin(); it != possibleConfigs.end(); it++)
	{
		std::vector<std::string> &serverNames = (*it)->getServerNames();
		for (std::vector<std::string>::iterator server_it = serverNames.begin(); server_it != serverNames.end(); server_it++)
		{
			std::cout << "Host: " << request_map["Host"] << "|" << std::endl;
			std::cout << "ServerName: " << *server_it << "|" << std::endl;
			std::string Host = request_map["Host"];
			std::string ServerName = *server_it;
			//config found
			if (Host == ServerName)
			{
				std::cout << "server name found" << std::endl;
				config = (*it);
				config->printConfig();
				return;
			}
		}
	}
	std::cout << "server name not found selecting first server from possible server" << std::endl;
	//if not found select first config for request
	config = possibleConfigs[0];
	config->printConfig();
}

//make a write request first and then check for availability to write to client_fd
void Request::HandleRequest()
{
	try
	{
		//if no route is found no response is generated
		Route *route = matchRoute(request_map["Path"]);
		if (!route)
			throw NotFoundException();
		std::cout << "Route: " << route->getPath() << std::endl;
		if (route->getRedirect().getUrl() != "")
			throw RedirectionException(route->getRedirect().getUrl(), route->getRedirect().getCode());
		std::string path = composePath(route);

		std::cout << MAGENTA << "Handling a " << request_map["Method"] << " request!" << RESET << std::endl;
		if (isCgiRequest(path))
			executeCGI(path);
		else if (request_map["Method"] == "GET")
		{
			std::string response = Handle_GET(path);
			_serverInstance->create_write_request(response, client->getClientFd());
		}
		else if (request_map["Method"] == "POST")
		{
			std::string response = Handle_POST(path, route);
			_serverInstance->create_write_request(response, client->getClientFd());
		}
		else if (request_map["Method"] == "DELETE")
			Handle_DELETE(path);
	}
	catch (const RedirectionException &e)
	{
		std::cerr << BLUE << "Redirection to " << e.getUrl() << RESET << std::endl;
		std::string responseBody = "<html><body><h1>You are being redirected</h1></body></html>";
		std::string responseHeader = "HTTP/1.1 " + std::to_string(e.getCode()) + " Moved\r\n";
		responseHeader += "Location: " + e.getUrl() + "\r\n";
		responseHeader += CONTENT_LENGTH + std::to_string(responseBody.size()) + "\r\n\r\n" + responseBody;
		_serverInstance->create_write_request(responseHeader, client->getClientFd());
	}
	catch (const ServerException &e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
		std::string response = getErrorPage(e);
		std::string responseHeader= getResponseCode(e) + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
		_serverInstance->create_write_request(responseHeader, client->getClientFd());
	}
}


std::string Request::getResponseCode(const ServerException &e)
{
	if (dynamic_cast<const NotFoundException *>(&e))
		return HTTP_NOT_FOUND;
	if (dynamic_cast<const BadRequestException *>(&e))
		return BAD_REQUEST;
	if (dynamic_cast<const ForbiddenException *>(&e))
		return HTTP_FORBIDDEN;
	if (dynamic_cast<const InternalServerErrorException *>(&e))
		return SERVER_ERROR;
	if (dynamic_cast<const MethodNotAllowedException *>(&e))
		return HTTP_METHOD_NOT_ALLOWED;
	if (dynamic_cast<const ContentTooLargeException *>(&e))
		return HTTP_CONTENT_TOO_LARGE;
	else
		return SERVER_ERROR;

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
	else if (dynamic_cast<const MethodNotAllowedException *>(&e))
		return (config->matchErrorPage(405));
	else if (dynamic_cast<const ContentTooLargeException *>(&e))
		return (config->matchErrorPage(413));
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
		std::cerr << "Error page not found" << std::endl;
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

std::string Request::normalizePath(std::string path)
{
	std::string normalizedPath = path;
	if (!normalizedPath.empty() && normalizedPath[0] == '/')
		normalizedPath.erase(0, 1);
	return normalizedPath;
}

std::string Request::handleFavicon()
{
	std::ifstream file("favicon/favicon.ico", std::ios::binary);
	if (file.is_open())
	{
		std::ostringstream ss;
		ss << file.rdbuf();
		std::string fileContent = ss.str();
		std::string response = HTTP_OK;
		response += CONTENT_LENGTH;
		response += std::to_string(fileContent.size());
		response += "\r\n\r\n";
		response += fileContent;
		return response;
	}
	std::cerr << "favicon.ico not found in favicon directory. Please make sure you have a favicon.ico file in the favicon directory" << std::endl;
	throw NotFoundException();
}
std::string Request::Handle_GET(std::string path)
{
	std::vector<std::string> allowed_methods = route->getAllowedMethods();
	if (std::find(allowed_methods.begin(), allowed_methods.end(), "GET") == allowed_methods.end() && !allowed_methods.empty())
		throw MethodNotAllowedException();
	if (request_map["Path"] == "/favicon.ico")
		return handleFavicon();

	std::cout << RED << "path: " << path << RESET << std::endl;
	std::cout << "autoindex: " << route->getAutoIndex() << std::endl;

	std::stringstream outstream;


	if (isDirectory(path))
	{
		std::cout << "path is an index" << std::endl;
		//if it is a directory we want the index file
		//if no index file is found return forbidden 403 error

		if (!route->getIndex().empty())
		{
			std::cout << "index specified" << std::endl;
			path.back() == '/' ? path += route->getIndex() : path += "/" + route->getIndex();
			std::ifstream file(path);
			if (file.is_open() && file.good())
				outstream << file.rdbuf();
			else
				throw ForbiddenException();
		}
		else if (route->getAutoIndex())
			outstream << createAutoIndex(path);
		else
			throw ForbiddenException();
	}
	else //requested URI is a file
	{
		std::ifstream file(path);
		if(!file.is_open() || !file.good())
			throw NotFoundException();
		outstream << file.rdbuf();
	}

	std::string response = HTTP_OK + CONTENT_LENGTH + std::to_string(outstream.str().size()) + "\r\n\r\n" + outstream.str();
	return response;
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