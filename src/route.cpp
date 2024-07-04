#include "../inc/Route.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>

Route::Route(std::vector<std::string>::iterator &it, std::vector<std::string>::const_iterator &end)
{
	autoindex = false;
	setPath(*it);
	it++;
	while (it != end)
	{
		std::string line = *it;
		std::cout << "line: " << line << std::endl;
		if (line.find('}') != std::string::npos)
			break ;
		std::istringstream ss(line);
		std::string key, value;
		ss >> key >> value;
		if (key == "limit_except")
			setAllowedMethods(line);
		else if (key == "alias")
			setAlias(value);
		else if (key == "index")
			setIndex(value);
		else if (key == "autoindex")
			setAutoIndex(value);
		else if (key == "client_body_temp_path")
			setUploadDir(value);
		else if (key == "return")
			setUpRedirect(line);
		else
		{
			std::cout << "Unknown directive: \"" << key << "\" inside location block" << std::endl;
			exit(1);
		}
		it++;
	}
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
	{
		if (value != "DELETE" && value != "GET" && value != "POST")
			{
				std::cerr << "Invalid method: \"" << value << "\" on line: " << line <<  std::endl;
				exit(1);
			}
		allowed_methods.push_back(value);
	}
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
	// std::cout << "autoindex: " << autoindex << std::endl;
}

void Route::setUploadDir(std::string upload_dir)
{
	this->upload_dir = upload_dir;
}

void Route::setUpRedirect(std::string line)
{
	std::istringstream iss(line);
	std::string returnKeyword, statusCodeString, url;

	if (!(iss >> returnKeyword >> statusCodeString >> url))
	{
		std::cerr << "Failed to parse line: " << line << std::endl;
		exit(1);
	}
	int code;
	try {
		code = std::stoi(statusCodeString);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
	if (code < 100 || code > 599)
	{
		std::cerr << "Invalid status code: " << code << std::endl;
		exit(1);
	}
	redirect = Redirect(url, code);
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

Redirect Route::getRedirect() const
{
	return (this->redirect);
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
	std::cout << "		upload_dir: " << getUploadDir() << std::endl;
	if (getAutoIndex() == true)
		std::cout << "		autoindex: on" << std::endl;
	else
		std::cout << "		autoindex: off" << std::endl;
	if (getRedirect().getUrl() != "")
		std::cout << "		redirect: " << getRedirect().getCode() << " " << getRedirect().getUrl() << std::endl;
	
}

