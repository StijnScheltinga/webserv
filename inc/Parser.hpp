#ifndef PARSER_HPP
# define PARSER_HPP

#include "Config.hpp"
#include <vector>
#include <string>

class Parser
{
	private:
		char						*configFilePath;
		std::vector<Config> 		configVector;
		void						createConfigObjects();
		void						getConfigFileString();
		int							lengthServerBlock(std::vector<std::string> configFileLines, int i);
		void						printConfigLines(std::vector<std::string> configLines);
		std::vector<std::string>	syntaxError();

	public:
		Parser(char *configFilePath);
		~Parser();

		std::vector<Config>			getConfigVector();
		
};

#endif