#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <map>
# include <fstream>
# include <sstream>
# include <vector>
# include "ErrorPage.hpp"
# include "Route.hpp"

class Config
{
	private:
		void ParseConfig();

		std::vector<std::string>	serverBlock;
		std::vector<std::string>	server_name;
		std::vector<Route>			routes;
		std::vector<ErrorPage> 		error_pages;


		int port;
		std::string root;
		size_t client_max_body_size;
		int	serverFd;

	public:
		Config(){;};
		Config(std::vector<std::string> serverBlock);
		~Config();
		void printConfig() const;
		void setServerName(std::string serverName);
		void setPort(std::string port);
		void setClientMaxBodySize(std::string clientMaxBodySize);
		void setErrorPage(std::string errorPage);
		void addRoute(std::vector<std::string>::iterator &it, std::vector<std::string>::const_iterator &end);
		void setRoot(std::string root);
		void setServerFd(int fd);
		int getServerFd();
		std::vector<Route> getRoutes();

		char getMaxClientBodySizeSuffix(std::string &clientMaxBodySize);

		int getPort() const;
		std::string getRoot() const;
		size_t getClientMaxBodySize() const;
		std::vector<ErrorPage> getErrorPages() const;

	std::map<std::string, void (Config::*)(std::string)> configHandlers = 
	{
		{"server_name", &Config::setServerName},
		{"listen", &Config::setPort},
		{"client_max_body_size", &Config::setClientMaxBodySize},
		{"error_page", &Config::setErrorPage},
		{"root", &Config::setRoot},
	};
	std::vector<std::string> cgiExtensions = {".py", ".cgi"};
};


#endif