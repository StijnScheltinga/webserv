#include <iostream>
#include "../inc/Request.hpp"


void Request::execute_cgi(std::string path)
{
	std::cout << "Executing CGI" << std::endl;
	int pipe_fd[2];
	if (pipe(pipe_fd) == -1)
	{
		std::cerr << "Failed to create pipe" << std::endl;
		return ;
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		std::cerr << "Failed to fork" << std::endl;
		return ;
	}
	if (pid == 0)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], 1);
		close(pipe_fd[1]);
		execve(path.c_str(), NULL, NULL);
	}
	else
	{
		close(pipe_fd[1]);
		char buffer[1024];
		int bytes_read;
		while ((bytes_read = read(pipe_fd[0], buffer, 1024)) > 0)
		{
			write(_client_socket, buffer, bytes_read);
		}
		close(pipe_fd[0]);
	}



}
bool Request::isCgiRequest(std::string path)
{

	try{
		std::string extension = path.substr(path.find_last_of('.'));
		std::cout << "Extension: " << extension << std::endl;
	
	for (std::vector<std::string>::iterator it = _config_map["CGIExtensions"].begin(); it != _config_map["CGIExtensions"].end(); it++)
	{
		if (*it == extension)
			return true;
	}
	}
	catch(const std::exception& e)
	{
		return false;
	}
	return false;
}