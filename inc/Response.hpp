#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "../inc/Request.hpp"
#include <string>
#include <iostream>
#include <map>

class Response
{
	private:
		int	clientSocket;
		std::map<std::string, std::string>	requestMap;
	public:
		Response(const Request& request);
		~Response();
		void	printMap();
};

#endif