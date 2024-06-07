#ifndef ERROR_HPP
# define ERROR_HPP

# define WEBSERV_ERRMAX 11

# include <iostream>

typedef enum webserv_errno
{
	OPEN_CONFIG_FAIL,
	COLON_EOL,
	SOCK_FAIL,
	BIND_FAIL,
	LISTEN_FAIL,
	EPOLL_ERROR,
	EPOLL_CTL_ERROR,
	EVENT_ERROR,
	READ_ERROR,
	ACCEPT_ERROR,
	SYNTAX_ERROR,

}t_webserv_errno;

static const char *g_webserv_errors[WEBSERV_ERRMAX] = 
{
	"Failed to open config file!",
	"No colon at the end of the line!",
	"Failed to create socket!",
	"Failed to bind socket to port!",
	"Failed to listen!",
	"Epoll error!",
	"Epoll_ctl error!",
	"Error getting events!",
	"Read error!",
	"accept error!",
	"syntax error!",
};

void	exitError(int webservErrno);

#endif