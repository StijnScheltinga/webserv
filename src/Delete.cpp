#include "../inc/Request.hpp"
#include "../inc/ServerException.hpp"
#include <algorithm>
#include <cstdio>

void	Request::Handle_DELETE(std::string path)
{
	std::vector<std::string> allowed_methods = route->getAllowedMethods();
	if (std::find(allowed_methods.begin(), allowed_methods.end(), "DELETE") == allowed_methods.end() && !allowed_methods.empty())
		throw MethodNotAllowedException();
	int result = remove(path.c_str());
	if (result == 0)
		return ;
	else
		throw InternalServerErrorException();
}