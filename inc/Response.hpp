#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "../inc/Request.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <exception>

const std::string HTTP_OK = "HTTP/1.1 200 OK\r\n";
const std::string HTTP_NOT_FOUND = "HTTP/1.1 404 Not Found\r\n";
const std::string BAD_REQUEST = "HTTP/1.1 400 Bad Request\r\n";
const std::string CONTENT_LENGTH = "Content-Length: ";


class Response
{
	public:
		Response();
		~Response();
		std::string getPage(std::string fileName);
};

#endif