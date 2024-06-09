#include "../inc/Request.hpp"
#include "../inc/ServerException.hpp"

std::string Request::find_file_name(std::string &request_string)
{
	size_t filename_pos = request_string.find("filename=\"");
	if (filename_pos == std::string::npos)
		return "";
	filename_pos += 10;
	size_t filename_end_pos = request_string.find("\"", filename_pos);
	return (request_string.substr(filename_pos, filename_end_pos - filename_pos));
}

std::string Request::find_boundary(std::string &request_string)
{
	size_t boundary_pos = request_string.find("boundary=");
	if (boundary_pos == std::string::npos)
		return "";
	boundary_pos += 9;
	size_t boundary_end_pos = request_string.find("\r\n", boundary_pos);
	return (request_string.substr(boundary_pos, boundary_end_pos - boundary_pos));
}

std::string Request::Handle_POST(Route *route)
{
	std::string response = "POST request received\n";
	std::string response_string = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
	std::string path;
	std::string request_string(requestString);
	std::string file_name = find_file_name(request_string);
	std::string boundary = "--" + find_boundary(request_string);
	if (route->getRoot().empty())
		path = config->getRoot() + route->getPath() + "/" + file_name;
	else
		path = route->getRoot() + route->getPath() + file_name;
	std::cout << "upload path: " << path << std::endl;
	std::ofstream ofs(path.c_str(), std::ios::binary);
	if (!ofs.is_open())
	{
		std::cout << "Error opening file" << std::endl;
		throw InternalServerErrorException();
	}
	size_t boundary_pos = request_string.find(boundary);
	if (boundary_pos == std::string::npos)
	{
		std::cerr << "Boundary not found" << std::endl;
		throw InternalServerErrorException();
	}
	size_t content_start = request_string.find("\r\n\r\n", boundary_pos) + 4;
	size_t content_end = request_string.find(boundary + "--", content_start) - 4;
	std::string content = request_string.substr(content_start, content_end - content_start);
	ofs.write(content.c_str(), content.size());
	ofs.close();
	return response_string;
}