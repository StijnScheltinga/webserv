#ifndef REQUEST_HPP
# define REQUEST_HPP
#include "../inc/Server.hpp"
#include "../inc/ServerException.hpp"
#include "../inc/Config.hpp"
#include <iostream>
#include <sstream>
#include <map>

const std::string HTTP_OK = "HTTP/1.1 200 OK\r\n";
const std::string HTTP_NOT_FOUND = "HTTP/1.1 404 Not Found\r\n";
const std::string BAD_REQUEST = "HTTP/1.1 400 Bad Request\r\n";
const std::string SERVER_ERROR = "HTTP/1.1 500 server error\r\n";
const std::string CONTENT_LENGTH = "Content-Length: ";

class Request
{
	public:
		Request(Client *client, Config *config, const char *requestString, Server *serverInstance);
		~Request();
		void ParseRequest();
		void HandleRequest();
		std::string Handle_GET(std::string path);
		std::string Handle_POST(std::string path, Route *route);
		std::string Handle_DELETE();
		std::string find_file_name(std::string &request_string);
		std::string find_boundary(std::string &request_string);
		std::string getErrorPage(const ServerException &e);
		std::string getErrorPath(const ServerException &e);
		void	printMap();
		
		bool isCgiRequest(std::string path);
		void execute_cgi(std::string path);
		Route *matchRoute(std::string path);
		std::string defineIndex(Route *route);
		bool isDirectory(std::string path);
		std::string normalizePath(std::string path);
		std::string composePath(Route *route);

	private:
		Client *client;
		Config *config;
		const char *requestString;
		int _client_fd;
		std::map<std::string, std::string> request_map;
		std::string _http_version;
		Server	*_serverInstance;
};

#endif