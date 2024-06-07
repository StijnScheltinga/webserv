#include <iostream>
#include "../inc/Request.hpp"


// void Request::execute_cgi(std::string path)
// {
// 	char *const argv[] = {NULL};
// 	char *const envp[] = {NULL};
// 	int pipe_fd[2];
// 	if (pipe(pipe_fd) == -1)
// 	{
// 		std::cerr << "Failed to create pipe" << std::endl;
// 		return ;
// 	}
// 	pid_t pid = fork();
// 	if (pid == -1)
// 	{
// 		std::cerr << "Failed to fork" << std::endl;
// 		return ;
// 	}
// 	if (pid == 0)
// 	{
// 		close(pipe_fd[0]);
// 		dup2(pipe_fd[1], 1);
// 		close(pipe_fd[1]);
// 		std::string cgi_script_path = _config_map["CGIDir"][0] + path;
// 		if (execve(cgi_script_path.c_str(), argv, envp) == -1)
// 		{
// 			perror("execve fail");
// 			exit(1);
// 		}
// 	}
// 	else
// 	{
// 		close(pipe_fd[1]);
// 		char buffer[1024];
// 		int bytes_read;
// 		std::string response;
// 		while ((bytes_read = read(pipe_fd[0], buffer, 1024)) > 0)
// 			response.append(buffer, bytes_read);
// 		std::string response_header = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
// 		write(_client_fd, response_header.c_str(), response_header.size());
// 		close(pipe_fd[0]);
// 	}
// }

// bool Request::isCgiRequest(std::string path)
// {

// 	try{
// 		std::string extension = path.substr(path.find_last_of('.'));
// 	for (std::vector<std::string>::iterator it = _config_map["CGIExtensions"].begin(); it != _config_map["CGIExtensions"].end(); it++)
// 	{
// 		if (*it == extension)
// 			return true;
// 	}
// 	}
// 	catch(const std::exception& e)
// 	{
// 		return false;
// 	}
// 	return false;
// }