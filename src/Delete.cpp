#include "../inc/Request.hpp"
#include "../inc/ServerException.hpp"
#include <cstdio>

std::string Request::Handle_DELETE()
{
	std::cout << "Path: " << request_map["Path"] << std::endl;
	int result = remove(request_map["Path"].c_str());
	if (result == 0)
		return "";
	else
		throw InternalServerErrorException();
}