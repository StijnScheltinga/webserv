#include "../inc/Request.hpp"
#include "../inc/ServerException.hpp"
#include <algorithm>

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
	{
		std::cerr << "Boundary not found\n";
		throw InternalServerErrorException();
	}
	boundary_pos += 9;
	size_t boundary_end_pos = request_string.find("\r\n", boundary_pos);
	return (request_string.substr(boundary_pos, boundary_end_pos - boundary_pos));
}

std::string Request::handlePlainText(std::string path)
{
	size_t contentStart = requestString.find("\r\n\r\n");
	if (contentStart == std::string::npos)
	{
		std::cerr << "Content not found" << std::endl;
		throw InternalServerErrorException();
	}
	std::string content = requestString.substr(contentStart + 4);
	if (content.size() > config->getClientMaxBodySize())
	{
		std::cerr << "File too large" << std::endl;
		throw ContentTooLargeException();
	}
	
	std::string upload_path = route->getUploadDir() + "/default_file";
	std::cout << "upload path: " << upload_path << std::endl;
	std::ofstream ofs(upload_path.c_str(), std::ios::binary);
	if (!ofs.is_open())
	{
		std::cerr << "Error opening file" << std::endl;
		throw InternalServerErrorException();
	}
	ofs << content;
	std::string response = "POST request received\n";
	std::string responseString = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
	return responseString;
}


std::string Request::handleMultiPart(std::string path)
{
	std::string request_string(requestString);
	std::string file_name = find_file_name(request_string);
	if (file_name.empty())
		file_name = "default_file";
	std::string boundary = "--" + find_boundary(request_string);
	std::string upload_path = route->getUploadDir() + "/" + file_name;
	std::ofstream ofs(upload_path.c_str(), std::ios::binary);

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
	size_t contentStart = request_string.find("\r\n\r\n", boundary_pos) + 4;
	size_t contentEnd = request_string.find(boundary + "--", contentStart);
	if (contentEnd == std::string::npos)
	{
		std::cerr << "End boundary not found" << std::endl;
		throw InternalServerErrorException();
	}
	contentEnd -= 4;
	std::string content = request_string.substr(contentStart, contentEnd - contentStart);
	if (content.size() > config->getClientMaxBodySize())
	{
		std::cerr << "File too large" << std::endl;
		throw ContentTooLargeException();
	}
	ofs.write(content.c_str(), content.size());

	std::vector<std::string> &uploaded_files = _serverInstance->getUploadedFiles();
	if (std::find(uploaded_files.begin(), uploaded_files.end(), upload_path) == uploaded_files.end())
	{
		std::cout << "uploading file: " << upload_path << " to server" << std::endl;
		uploaded_files.push_back(upload_path);
	}
	else
		std::cout << "file already exists" << std::endl;
	
	std::string response = "POST request received\n";
	std::string responseString= HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
	return responseString;
}

std::string Request::Handle_POST(std::string path, Route *route)
{
	std::vector <std::string> allowed_methods = route->getAllowedMethods();
	if (std::find(allowed_methods.begin(), allowed_methods.end(), "POST") == allowed_methods.end() && !allowed_methods.empty())
		throw MethodNotAllowedException();

	if (request_map["Content-Type"].find("text/html") != std::string::npos)
		return handlePlainText(path);
	else
		return handleMultiPart(path);
}