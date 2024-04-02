#include "../inc/Response.hpp"

Response::Response(const Request& request)
{
	this->clientSocket = request.getClientSocket();
	this->requestMap = request.getRequestMap();

}

Response::~Response() {}

void	Response::printMap(void)
{
	std::map<std::string, std::string>::iterator	it = requestMap.begin();

	std::cout << "---request---\n";
	for (;it != requestMap.end(); it++)
		std::cout << "key: " << it->first << ", value: " << it->second << "\n";
	std::cout << "---end request---" << std::endl;
}