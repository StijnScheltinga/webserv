#include "../inc/Request.hpp"
#include "../inc/Response.hpp"
#include <iostream>
#include <unistd.h>

Request::Request(int client_socket, const char *buffer)
{
	_client_socket = client_socket;
	_buffer = buffer;
}

Request::~Request()
{
}

void Request::ParseRequest()
{
	//First line always has the method, path and version.
	std::istringstream ss(_buffer);
	std::string method, path, version;
	ss >> method >> path >> version;
	request_map["Method"] = method;
	request_map["Path"] = path;
	request_map["Version"] = version;

	std::string line;
	//skip the first line, it is parsed already.
	std::getline(ss, line);
	while (std::getline(ss, line) && line != "\r")
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

std::string Request::Handle_GET()
{
	Response	response;
	return response.getPage(request_map["Path"]);
}

std::string Request::Handle_POST(std::string body)
{
	return body;
}

std::string Request::Handle_DELETE()
{
	return "";
}
void Request::HandleRequest()
{
	printMap();
	try
	{
		if (request_map["Method"] == "GET")
		{
			std::string response = Handle_GET();
			std::string response_header = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
			write(_client_socket, response_header.c_str(), response_header.size());
		}
		else if (request_map["Method"] == "POST")
		{
			std::string response = Handle_POST(request_map["Body"]);
			std::string response_header = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
			write(_client_socket, response_header.c_str(), response_header.size());

		}
		else if (request_map["Method"] == "DELETE")
		{
			
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "improper request" << std::endl;
		write(_client_socket, BAD_REQUEST.c_str(), BAD_REQUEST.size());
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
