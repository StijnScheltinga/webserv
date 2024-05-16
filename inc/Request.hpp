#ifndef REQUEST_HPP
# define REQUEST_HPP
#include "../inc/Server.hpp"
#include <iostream>
#include <sstream>
#include <map>

const std::string HTTP_OK = "HTTP/1.1 200 OK\r\n";
const std::string HTTP_NOT_FOUND = "HTTP/1.1 404 Not Found\r\n";
const std::string BAD_REQUEST = "HTTP/1.1 400 Bad Request\r\n";
const std::string CONTENT_LENGTH = "Content-Length: ";

class Request
{
	public:
		Request(int client_socket, const char *buffer, std::map<std::string, std::vector<std::string> > config_map);
		~Request();
		void ParseRequest();
		void HandleRequest(std::string &request_string);
		std::string Handle_GET();
		std::string Handle_POST(std::string &request_string);
		std::string Handle_DELETE();
		std::string find_file_name(std::string &request_string);
		std::string find_boundary(std::string &request_string);
		void	printMap();
		void	printRequest();
		
		bool isCgiRequest(std::string path);
		void execute_cgi(std::string path);

	private:
		const char *_buffer;
		int _client_socket;
		std::map<std::string, std::vector<std::string> > _config_map;
		std::map<std::string, std::string> request_map;
		std::string _http_version;
};

#endif