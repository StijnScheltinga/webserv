#include "../inc/Request.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>

Request::Request(int client_fd, const char *buffer, std::map<std::string, std::vector<std::string> > config_map, Server *serverInstance)
{
	_client_fd = client_fd;
	_buffer = buffer;
	_config_map = config_map;
	_serverInstance = serverInstance;
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
	size_t filename_pos = request_string.find("filename=\"");
	if (filename_pos == std::string::npos)
		return "";
	filename_pos += 10;
	size_t filename_end_pos = request_string.find("\"", filename_pos);
	return (request_string.substr(filename_pos, filename_end_pos - filename_pos));
}

std::string Request::find_boundary(std::string &request_string)
{
	size_t boundary_pos = request_string.find("boundary=");
	if (boundary_pos == std::string::npos)
		return "";
	boundary_pos += 9;
	size_t boundary_end_pos = request_string.find("\r\n", boundary_pos);
	return (request_string.substr(boundary_pos, boundary_end_pos - boundary_pos));
}
std::string Request::Handle_POST(std::string &request_string)
{
	std::string response = "POST request received\n";
	std::string response_string = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
	std::string filename = _config_map["UploadDir"][0] + "/" + find_file_name(request_string);
	std::string boundary = "--" + find_boundary(request_string);
	std::ofstream ofs(filename.c_str(), std::ios::binary);
	if (!ofs.is_open())
	{
		std::cout << "Error opening file" << std::endl;
		return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
	}
	size_t boundary_pos = request_string.find(boundary);
	if (boundary_pos == std::string::npos)
	{
		std::cerr << "Boundary not found" << std::endl;
		return BAD_REQUEST;
	}
	size_t content_start = request_string.find("\r\n\r\n", boundary_pos) + 4;
	size_t content_end = request_string.find(boundary + "--", content_start) - 4;
	std::string content = request_string.substr(content_start, content_end - content_start);
	ofs.write(content.c_str(), content.size());
	ofs.close();
	return response_string;
}

std::string Request::Handle_DELETE()
{
	return "";
}

/*for get method instead of writing directly WITHOUT poll, 
make a write request first and then check for availability to write to client_fd*/
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
			//create write request
			_serverInstance->create_write_request(response_header, _client_fd);
			// write(_client_fd, response_header.c_str(), response_header.size());
		}
		else if (request_map["Method"] == "POST")
		{
			std::string response = Handle_POST(request_string);
			write(_client_fd, response.c_str(), response.size());
		}
		else if (request_map["Method"] == "DELETE")
		{
			
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "improper request" << std::endl;
		std::cout << "response: " << _client_fd << BAD_REQUEST.c_str() << BAD_REQUEST.size() << std::endl;
		write(_client_fd, BAD_REQUEST.c_str(), BAD_REQUEST.size());
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