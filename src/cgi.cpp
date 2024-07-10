#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include "../inc/Request.hpp"

#define MAX_CGI_EXECUTION_TIME 2 // seconds


void Request::executeCGI(std::string path)
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
		pid_t result;
		int status;
		time_t start_time = time(nullptr);

		//wait for child process to finish
		while (true)
		{
			result = waitpid(pid, &status, WNOHANG);
			if (result == 0)
			{
				if (time(nullptr) - start_time >= MAX_CGI_EXECUTION_TIME)
				{
					kill(pid, SIGKILL);
					waitpid(pid, &status, 0);
					std::cerr << "CGI script execution time exceeded" << std::endl;
					throw InternalServerErrorException();
				}
			}
			else if (result == -1)
			{
				std::cerr << "Waitpid fail" << std::endl;
				throw InternalServerErrorException();
			}
			else
			{
				if (WIFEXITED(status))
				{
					if (WEXITSTATUS(status) != 0)
					{
						std::cerr << "CGI script exited with status " << WEXITSTATUS(status) << std::endl;
						throw InternalServerErrorException();
					}
					break;
				}
				if (WIFSIGNALED(status))
				{
					std::cerr << "CGI script was terminated by signal " << WTERMSIG(status) << std::endl;
					throw InternalServerErrorException();
				}
				break;
			}

		}
		std::string response;
		while ((bytes_read = read(pipe_fd[0], buffer, 1024)) > 0)
			response.append(buffer, bytes_read);
		std::string response_header = HTTP_OK + CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
		_serverInstance->create_write_request(response_header, client->getClientFd());
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