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
		std::string root;
		bool 		autoindex;
		std::string index;;
		std::string upload_dir;
		CgiConfig	cgi_config;
		

	public:
		Route();
		~Route();
		void setPath(std::string path);
		void setAllowedMethods(std::string line);
		void setRoot(std::string root);
		void setIndex(std::string index);
		void setAutoIndex(std::string autoindex);

		std::string getPath() const;
		std::vector<std::string> getAllowedMethods() const;
		std::string getRoot() const;
		std::string getIndex() const;
		bool getAutoIndex() const;
		// void setUploadDir(std::string upload_dir);
		// void setCgiConfig(CgiConfig cgi_config);

		void printRoute() const;

};

#endif