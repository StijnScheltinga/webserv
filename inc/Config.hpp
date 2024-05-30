#ifndef CONFIG_HPP
# define CONFIG_HPP
# include <iostream>
# include <map>
# include <fstream>
# include <sstream>

class Config
{
	public:
		Config(const char *configFile);
		~Config();
		void ParseConfig(std::string serverBlock);
		void printConfig() const;

	private:
		
		std::string listen;
		std::string server_name;
		std::string error_page;
		std::string max_client_body_size;

};


#endif