#ifndef PARSER_HPP
# define PARSER_HPP

#include "Config.hpp"
#include <vector>
#include <string>

class Parser
{
	public:
		Parser(char *configFilePath);
		~Parser();
		std::vector<Config>	getConfigVector() const;
	private:
		char		*configFilePath;
		std::string	configFileString;
		std::vector<Config> configVector;
		void	createConfigObjects();
		void	getConfigFileString();
		void	copyServerBlock(std::ifstream &configStream, std::string &line, std::string &configContentString);
		
};

#endif