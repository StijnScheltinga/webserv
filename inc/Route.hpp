#ifndef ROUTE_HPP
# define ROUTE_HPP

# include <string>
# include <vector>
# include "CgiConfig.hpp"

class Route
{
	private:
		std::string path;
		std::vector<std::string> allowed_methods;
		std::string alias;
		bool 		autoindex;
		std::string index;;
		std::string upload_dir;
		CgiConfig	cgi_config;
		

	public:
		Route();
		~Route();
		void setPath(std::string path);
		void setAllowedMethods(std::string line);
		void setAlias(std::string alias);
		void setIndex(std::string index);
		void setAutoIndex(std::string autoindex);
		void setUploadDir(std::string upload_dir);

		std::string getPath() const;
		std::vector<std::string> getAllowedMethods() const;
		std::string getAlias() const;
		std::string getIndex() const;
		bool getAutoIndex() const;
		std::string getUploadDir() const;
		// void setUploadDir(std::string upload_dir);
		// void setCgiConfig(CgiConfig cgi_config);

		void printRoute() const;
};

#endif