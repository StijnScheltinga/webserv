#include "../inc/Route.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>

Route::Route()
{
	autoindex = false;
}

Route::~Route()
{
}

void Route::setPath(std::string line)
{
	std::istringstream iss(line);
	std::string path, location;
	iss >> location >> this->path;
}

void Route::setAllowedMethods(std::string line)
{
	std::istringstream iss(line);
	std::string allowedMethodsKeyword, value;
	iss >> allowedMethodsKeyword;

	while (iss >> value)
		allowed_methods.push_back(value);
}

void Route::setAlias(std::string alias)
{
	this->alias = alias;
}

void Route::setIndex(std::string index)
{
	this->index = index;
}

void Route::setAutoIndex(std::string autoindex)
{
	if (autoindex == "on")
		this->autoindex = true;
	else
		this->autoindex = false;
	
}

void Route::setUploadDir(std::string upload_dir)
{
	this->upload_dir = upload_dir;
}

// void Route::setCgiConfig(CgiConfig cgi_config)
// {
// 	this->cgi_config = cgi_config;
// }

std::string Route::getPath() const
{
	return (this->path);
}

std::vector<std::string> Route::getAllowedMethods() const
{
	return (this->allowed_methods);
}

std::string Route::getAlias() const
{
	return (this->alias);
}

std::string Route::getIndex() const
{
	return (this->index);
}

bool Route::getAutoIndex() const
{
	return (this->autoindex);
}

std::string Route::getUploadDir() const
{
	return (this->upload_dir);

}

void Route::printRoute() const
{
	std::cout << "	path: " << getPath() << std::endl;
	std::cout << "		allowed methods: ";
	std::vector<std::string> allowedMethods = getAllowedMethods();
	for (size_t j = 0; j < allowedMethods.size(); j++)
		std::cout << "			" << allowedMethods[j] << " ";
	std::cout << std::endl;
	std::cout << "		alias: " << getAlias() << std::endl;
	std::cout << "		index: " << getIndex() << std::endl;
	if (getAutoIndex() == true)
		std::cout << "		autoindex: on" << std::endl;
	else
		std::cout << "		autoindex: off" << std::endl;
}

