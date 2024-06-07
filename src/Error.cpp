#include "../inc/Error.hpp"

void	exitError(int webservErrno)
{
	std::cout << g_webserv_errors[webservErrno] << std::endl;
	exit(webservErrno);
}

