#include "../inc/ErrorPage.hpp"
#include <iostream>
#include <sstream>

ErrorPage::ErrorPage(std::string errorPage)
{
	std::istringstream	ss(errorPage);
	std::string 		token;
	while (ss >> token)
	{
		if (token[0] == '/')
		{
			this->setPath(token);
			break ;
		}
		else
		{
			this->addStatusCode(token);
		}
	}
}

ErrorPage::~ErrorPage()
{
}

std::string	ErrorPage::getPath()
{
	return (this->path);
}

void	ErrorPage::setPath(std::string path)
{
	this->path = path;
}

std::vector<int> ErrorPage::getStatusCodesVector()
{
	return (this->statusCodes);
}

void ErrorPage::addStatusCode(std::string statusCode)
{
	size_t length = statusCode.length();
	if (length == 0)
	{
		std::cerr << "Please provide a number for status code" << std::endl;
		exit(1);
	}
	for (size_t i = 0; i < length; i++)
	{
		if (!std::isdigit(statusCode[i]))
		{
			std::cerr << "status code must be a number" << std::endl;
			exit(1);
		}
	}
	this->statusCodes.push_back(std::stoi(statusCode));
}
