#ifndef REQUEST_HPP
# define REQUEST_HPP
#include <iostream>
#include <sstream>
#include <map>

class Request
{
	public:
		Request(int client_socket, const char *buffer);
		~Request();
		void ParseRequest();
		void HandleRequest();

	private:
		const char *_buffer;
		int _client_socket;
		std::map<std::string, std::string> request_map;
		std::string _http_version;
};

#endif