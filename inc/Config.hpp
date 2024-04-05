#include <iostream>
#include <map>
#include <fstream>
#include <sstream>

class Config
{
	public:
		Config();
		~Config();
		void ParseConfig(std::string file);
		void printConfig();
		void printMap();

	private:
		std::map<std::string, std::string> config_map;
		std::string listen;
		std::string server_name;
		std::string error_page;
		std::string max_client_body_size;

};