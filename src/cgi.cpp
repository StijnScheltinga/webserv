#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include "../inc/Request.hpp"
#include <string.h>

#define MAX_CGI_EXECUTION_TIME 2 // seconds


void Request::executeCGI(std::string path)
{
	std::cout << "path: " << path << std::endl;
	char *const argv[] = {NULL};
	char *const envp[] = {NULL};
	int pipeFd[2];
	if (pipe(pipeFd) == -1)
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
		std::string postData = requestString.substr(requestString.find("\r\n\r\n") + 4);
		close(pipeFd[0]);
		dup2(pipeFd[1], 1);
		close(pipeFd[1]);
		
		// setenv("REQUEST_METHOD", request_map["Method"].c_str(), 1);
		// setenv("REQUEST_URI", request_map["Path"].c_str(), 1);
		// setenv("CONTENT_LENGTH", std::to_string(postData.size()).c_str(), 1);
		// setenv("SCRIPT_NAME", path.c_str(), 1);
		// setenv("CONTENT_LENGTH", std::to_string(postData.size()).c_str(), 1);
		// setenv("CONTENT_TYPE", "text/html", 1);

		int postFd[2];
		if (pipe(postFd) == -1)
		{
			std::cerr << "Failed to create pipe" << std::endl;
			throw InternalServerErrorException();
		}
		pid_t postPid = fork();
		if (postPid == -1)
		{
			std::cerr << "Failed to fork" << std::endl;
			throw InternalServerErrorException();
		}

		if (postPid == 0)
		{
			close(postFd[0]);
			write(postFd[1], postData.c_str(), postData.size());
			close(postFd[1]);
			exit(0);
		}
		else
		{
			close(postFd[1]);
			dup2(postFd[0], 0);
			close(postFd[0]);
			if (execve(path.c_str(), argv, envp) == -1)
			{
				std::cerr << "Failed to execute CGI script" << std::endl;
				exit(1);
			}
		}
		
	}
	else
	{
		close(pipeFd[1]);
		char buffer[1024];
		int bytesRead;
		pid_t result;
		int status;
		time_t startTime = time(nullptr);

		while (true)
		{
			result = waitpid(pid, &status, WNOHANG);
			if (result == 0)
			{
				// Check if the CGI script has been running for too long
				if (time(nullptr) - startTime >= MAX_CGI_EXECUTION_TIME)
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
				// CGI script exited
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
		while ((bytesRead = read(pipeFd[0], buffer, 1024)) > 0)
			response.append(buffer, bytesRead);
		std::string responseString = HTTP_OK + "Content-Type: text/html\r\n" +  CONTENT_LENGTH + std::to_string(response.size()) + "\r\n\r\n" + response;
		_serverInstance->create_write_request(responseString, client->getClientFd());
		close(pipeFd[0]);
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