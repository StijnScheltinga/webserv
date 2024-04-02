#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "../inc/Request.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <exception>

class Response
{
	public:
		Response();
		~Response();
		std::string getPage(std::string fileName);
};

#endif