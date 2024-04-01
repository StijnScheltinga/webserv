#include "../inc/Request.hpp"
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

void Request::HandleRequest()
{
	std::ostringstream ss;
	ss << "Received request: " << request_map["Method"] << " " << request_map["Path"] << " " << _http_version;
	std::cout << ss.str() << std::endl;
	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 12\r\n\r\nHello world!";
	write(_client_socket, response.c_str(), response.size());
}