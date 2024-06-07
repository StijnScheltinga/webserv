#ifndef CGI_CONFIG_HPP
# define CGI_CONFIG_HPP


# include <string>

class CgiConfig
{
	private:
		std::string path;
		std::string extension;
	

	public:
		CgiConfig();
		~CgiConfig();
		std::string getPath() const;
		std::string getExtension() const;
		void setPath(std::string path);
		void setExtension(std::string extension);
};

#endif