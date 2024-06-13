#include <iostream>
#include "../inc/Request.hpp"


void Request::execute_cgi(std::string path)
{
	char *const argv[] = {NULL};
	char *const envp[] = {NULL};
	int pipe_fd[2];
	if (pipe(pipe_fd) == -1)
	{
		std::cerr << "Failed to create pipe" << std::endl;
		throw InternalServerErrorException();
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		std::cerr << "Failed to fork" << std::endl;
		throw InternalServerErrorException();
	}
	if (pid == 0)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], 1);
		close(pipe_fd[1]);
		if (execve(path.c_str(), argv, envp) == -1)
		{
			std::cerr << "Failed to execute CGI script" << std::endl;
			throw InternalServerErrorException();
		}
	}
	else
	{
		close(pipe_fd[1]);
		char buffer[1024];
		int bytes_read;
		std::string response;
		while ((bytes_read = read(pipe_fd[0], buffer, 1024)) > 0)
			response.append(buffer, bytes_read);
		std::string response_header = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
		write(_client_fd, response_header.c_str(), response_header.size());
		close(pipe_fd[0]);
	}
}

bool Request::isCgiRequest(std::string path)
{

	size_t lastDot = path.find_last_of('.');
	if (lastDot == std::string::npos)
		return false;
	std::string extension = path.substr(lastDot);
	for (std::vector<std::string>::iterator it = config->cgiExtensions.begin(); it != config->cgiExtensions.end(); it++)
	{
		if (*it == extension)
			return true;
	}
	return false;
}