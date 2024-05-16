#include "../inc/Request.hpp"
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

std::string Request::Handle_GET()
{
	std::string	fileName(request_map["Path"]);
	
	std::cout << "Path: " << fileName << std::endl;

	if (fileName == "/")
		fileName += "index.html";
	std::string	path = "webSrc" + fileName;
	std::ifstream	file(path);
	if (file.is_open() && file.good())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		// std::cout << buffer.str() << std::endl;
		return buffer.str();
	}
	else
	{
		std::cout << "Throwing exception\n";
		throw std::exception();
	}
}
std::string Request::find_file_name(std::string &request_string)
{
	std::string filename;
	if (request_string.find("filename=") != std::string::npos)
	{
		filename = request_string.substr(request_string.find("filename=") + 9);
		filename = filename.substr(1, filename.find("\r\n") - 2);
	}
	return filename;
}

std::string Request::find_boundary(std::string &request_string)
{
	if (request_string.find("boundary=") != std::string::npos)
	{
		std::string boundary;
		boundary = request_string.substr(request_string.find("boundary=") + 9);
		boundary = boundary.substr(0, boundary.find("\r\n"));
		return boundary;
	}
	return "";
}
std::string Request::Handle_POST(std::string &request_string)
{
	std::string response = "POST request received\n";
	std::string response_string = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
	std::string filename = _config_map["UploadDir"][0] + "/" + find_file_name(request_string);
	std::string boundary = "--" + find_boundary(request_string);
	std::cout << "filename: " << filename << std::endl;
	std::cout << "boundary: " << boundary << std::endl;
	int fd = open(filename.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd == -1)
	{
		std::cerr << "Failed to open file" << std::endl;
		return "Failed to open file\n";
	}
	std::string content;
	size_t boundary_pos = request_string.find(boundary);
	if (boundary_pos != std::string::npos)
	{
		content = request_string.substr(boundary_pos + boundary.size());
		for (int i  = 0; i < 4; i++)
		{
			content = content.substr(content.find("\r\n") + 2);
		}
	}
	size_t end_boundary_pos = content.find(boundary);
	if (end_boundary_pos != std::string::npos)
	{
		content = content.substr(0, end_boundary_pos);
	}

	write(fd, content.c_str(), content.size());
	close(fd);
	return response_string;


}

std::string Request::Handle_DELETE()
{
	return "";
}
void Request::HandleRequest(std::string &request_string)
{
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
			std::string response = Handle_POST(request_string);
			write(_client_socket, response.c_str(), response.size());
		}
		else if (request_map["Method"] == "DELETE")
		{
			
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "improper request" << std::endl;
		std::cout << "response: " << _client_socket << BAD_REQUEST.c_str() << BAD_REQUEST.size() << std::endl;
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