#include "../inc/Response.hpp"

Response::Response() {}
Response::~Response() {}

//returns a HTML page from webSrc directory
std::string	Response::getPage(std::string fileName)
{
	std::string	path = "webSrc" + fileName + ".html";
	std::cout << "Path getPage: " << path << std::endl;
	std::ifstream	file(path);
	if (file.is_open() && file.good())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		// std::cout << buffer.str() << std::endl;
		file.close();
		return buffer.str();
	}
	else
		throw std::exception();
}