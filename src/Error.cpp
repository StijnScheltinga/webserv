#include "../inc/Request.hpp"

std::string Request::getErrorPage(std::string filename)
{
	std::string path = _config_map["ErrorPages"][0] + "/" + filename;
	std::ifstream file(path);
	if (file.is_open() && file.good())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}
}