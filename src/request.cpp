#include "../inc/Request.hpp"
#include "../inc/Response.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

Request::Request(int client_socket, const char *buffer, std::map<std::string, std::vector<std::string> > config_map)
{
	_client_socket = client_socket;
	_buffer = buffer;
	_config_map = config_map;
}

Request::~Request()
{
}

void Request::ParseRequest()
{
	printRequest();
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
	std::cout << "Path: " << request_map["Path"] << std::endl;
	return response.getPage(request_map["Path"]);
}

std::string Request::Handle_POST(std::string body)
{
	// I want to upload the file to the server
	std::string response = "POST request received\n";
	std::string response_string = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
	std::string filename = "uploads" + request_map["Path"];
	int fd = open(filename.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd == -1)
	{
		std::cerr << "Failed to open file" << std::endl;
		return "";
	}
	write(fd, body.c_str(), body.size());
	close(fd);
	return response;


}

std::string Request::Handle_DELETE()
{
	return "";
}
void Request::HandleRequest()
{
	std::cout << "Accepted a " << request_map["Method"] << " request!" << std::endl;
	try
	{
		if (isCgiRequest(request_map["Path"]))
		{
			std::cout << "Executing CGI" << std::endl;
			execute_cgi(request_map["Path"]);
		}
		else if (request_map["Method"] == "GET")
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

void	Request::printRequest(void)
{
	std::cout << _buffer << std::endl;
}