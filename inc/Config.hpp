#ifndef CONFIG_HPP
# define CONFIG_HPP
# include <iostream>
# include <map>
# include <fstream>
# include <sstream>

struct ServerBlock
{
	std::map<std::string, std::vector<std::string> > config_map;
};

class Config
{
	public:
		Config();
		~Config();
		void ParseConfig(std::string file);
		void printConfig();
		void printMap();

	private:
		std::map<std::string, std::vector<std::string> > config_map;
		std::string listen;
		std::string server_name;
		std::string error_page;
		std::string max_client_body_size;

};

ServerBlock ParseConfig(std::string file);

#endif