#include "../inc/Request.hpp"
#include "../inc/ServerException.hpp"
#include "../inc/Config.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>

Request::Request(Client *client, Config *config, const char *requestString, Server	*serverInstance)
{
	this->client = client;
	this->config = config;
	this->requestString = requestString;
	this->serverInstance = serverInstance;
	ParseRequest();
	printMap();
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


//make a write request first and then check for availability to write to client_fd
void Request::HandleRequest()
{
	try
	{
		std::cout << MAGENTA << "Handling a " << request_map["Method"] << " request!" << RESET << std::endl;
		// if (isCgiRequest(request_map["Path"]))
		// 	execute_cgi(request_map["Path"]);
		if (request_map["Method"] == "GET")
		{
			std::string response = Handle_GET();
			std::string response_header = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
			//create write request
			serverInstance->create_write_request(response_header, client->getClientFd());
		}
		// else if (request_map["Method"] == "POST")
		// {
		// 	std::string postRequestString(requestString);
		// 	std::string response = Handle_POST(postRequestString);
		// 	std::string response_header = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
		// 	_serverInstance->create_write_request(response, _client_fd);
		// }
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
		serverInstance->create_write_request(response_header, client->getClientFd());
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
	if (dynamic_cast<)
}

std::string Request::Handle_GET()
{
	std::string	fileName(request_map["Path"]);

	if (fileName == "/")
		fileName += "index.html";
	std::string path = config->getRoot() + fileName;
	std::ifstream	file(path);
	if (file.is_open() && file.good())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}
	else
	{
		std::cout << "Throwing exception\n";
		throw NotFoundException();
	}
}