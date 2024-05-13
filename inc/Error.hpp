#ifndef ERROR_HPP
# define ERROR_HPP

# define WEBSERV_ERRMAX 6

typedef enum webserv_errno
{
	OPEN_CONFIG_FAIL,
	COLON_EOL,
	SOCK_FAIL,
	BIND_FAIL,
	LISTEN_FAIL,

}t_webserv_errno;

static const char *g_webserv_errors[WEBSERV_ERRMAX] = 
{
	"Failed to open config file!",
	"No colon at the end of the line!",
	"Failed to create socket!",
	"Failed to bind socket to port!",
	"Failed to listen!",

};

#endif