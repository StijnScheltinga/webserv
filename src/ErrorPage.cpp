#include "../inc/ErrorPage.hpp"
#include <iostream>
#include <sstream>

ErrorPage::ErrorPage(std::string errorPage)
{
	std::istringstream	ss(errorPage);
	std::string 		token;
	int	arg_n = 0;

	if (ss >> token)
	{
		int i = 0;
		for (char c : token)
		{
			if (i > 2)
			{
				std::cerr << "Error code can only comprise of 3 numbers" << std::endl;
				exit(1);
			}
			if (!isdigit(c))
			{
				std::cerr << "Error code can only comprise of numbers" << std::endl;
				exit(1);
			}
			i++;	
		}
		this->addStatusCode(token);
	}
	else
	{
		std::cerr << "Error page directive has no status code" << std::endl;
		exit(1);
	}
	if (ss >> token)
		this->setPath(token);
	else
	{
		std::cerr << "Error page directive has no path" << std:: endl;
		exit (1);
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

void ErrorPage::printErrorPage() const
{
	std::cout << "	Path: " << this->path << std::endl;
	for (size_t i = 0; i < this->statusCodes.size(); i++)
	{
		std::cout << "		Status code: " << this->statusCodes[i] << std::endl;
	}
}
