#include "../inc/Request.hpp"
#include "../inc/ServerException.hpp"
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

Request::~Request() {}

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

/*for get method instead of writing directly WITHOUT poll, 
make a write request first and then check for availability to write to client_fd*/
void Request::HandleRequest(std::string &request_string)
{
	try
	{
		std::cout << MAGENTA << "Handling a " << request_map["Method"] << " request!" << RESET << std::endl;
		if (isCgiRequest(request_map["Path"]))
			execute_cgi(request_map["Path"]);
		else if (request_map["Method"] == "GET")
		{
			std::string response = Handle_GET();
			std::string response_header = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
			//create write request
			_serverInstance->create_write_request(response_header, _client_fd);
		}
		else if (request_map["Method"] == "POST")
		{
			std::string response = Handle_POST(request_string);
			std::string response_header = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
			_serverInstance->create_write_request(response, _client_fd);
		}
		else if (request_map["Method"] == "DELETE")
		{
			Handle_DELETE();
		}
	}
	catch (const BadRequestException &e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
		std::string response = getErrorPage("400.html");
		std::string response_header = BAD_REQUEST + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
		_serverInstance->create_write_request(response_header, _client_fd);
	}
	catch (const NotFoundException &e){
		std::cerr << RED <<  e.what() << RESET <<  std::endl;
		std::string response = getErrorPage("404.html");
		std::string response_header = HTTP_NOT_FOUND + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
		_serverInstance->create_write_request(response_header, _client_fd);
	}
	catch (const InternalServerErrorException &e){
		std::cerr << RED << e.what() << RESET << std::endl;
		std::string response = getErrorPage("500.html");
		std::string response_header = SERVER_ERROR + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
		_serverInstance->create_write_request(response_header, _client_fd);
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