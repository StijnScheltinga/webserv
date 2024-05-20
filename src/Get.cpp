#include "../inc/Request.hpp"

std::string Request::Handle_GET()
{
	std::string	fileName(request_map["Path"]);

	if (fileName == "/")
		fileName += "index.html";
	std::string	path = _config_map["WebSource"][0] + "/" + fileName;
	std::ifstream	file(path);
	if (file.is_open() && file.good())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		// std::cout << buffer.str() << std::endl;
		return buffer.str();
	}
	else
	{
		std::cout << "Throwing exception\n";
		throw std::exception();
	}
}