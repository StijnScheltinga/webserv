#include "../inc/Response.hpp"

Response::Response() {}
Response::~Response() {}

std::string	Response::getPage(std::string fileName)
{
	std::string	path = "../webSrc" + fileName + ".html";
	std::cout << "! " << path << std::endl;
	std::ifstream	file(path);
	if (!file.good())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::cout << buffer.str() << std::endl;
		return buffer.str();
	}
	else
		throw std::exception();
}