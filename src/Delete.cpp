#include "../inc/Request.hpp"
#include "../inc/ServerException.hpp"
#include <algorithm>
#include <cstdio>
#include <dirent.h>

bool Request::deleteFile(std::string path)
{
	if (unlink(path.c_str()) == 0)
		return true;
	else
		return false;
}
void	Request::Handle_DELETE(std::string path)
{
	//check if DELETE is allowed
	std::vector<std::string> allowed_methods = route->getAllowedMethods();
	if (std::find(allowed_methods.begin(), allowed_methods.end(), "DELETE") == allowed_methods.end() && !allowed_methods.empty())
		throw MethodNotAllowedException();
	
	// Check if file is in uploaded files
	std::vector<std::string> &uploaded_files = _serverInstance->getUploadedFiles();
	if (std::find(uploaded_files.begin(), uploaded_files.end(), path) == uploaded_files.end())
	{
		std::cerr << "File not found in uploaded files" << std::endl;
		throw ForbiddenException();
	}
	// Check if file exists in filesystem
	if (!deleteFile(path))
	{
		std::cerr << "Error deleting file" << std::endl;
		throw InternalServerErrorException();
	}
	// Remove file from uploaded files
	for (std::vector<std::string>::iterator it = uploaded_files.begin(); it != uploaded_files.end(); it++)
	{
		if (*it == path)
		{
			uploaded_files.erase(it);
			break;
		}
	}
}