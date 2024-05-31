#ifndef CONFIG_HPP
# define CONFIG_HPP
# include <iostream>
# include <map>
# include <fstream>
# include <sstream>
# include <vector>

class Config
{
	public:
		Config(std::vector<std::string> serverBlock);
		~Config();
		void printConfig() const;

	private:
		void ParseConfig();

		std::vector<std::string> serverBlock;
};


#endif