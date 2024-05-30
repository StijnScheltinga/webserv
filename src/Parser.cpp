#include "../inc/Parser.hpp"
#include "../inc/Error.hpp"
#include "../inc/Server.hpp"

Parser::Parser(char *configPath) : configFilePath(configPath) {createConfigObjects();}

Parser::~Parser() {}

//call Config constructor for every server block
//config class handles parsing of info inside Server block
//Put config object in the configVector

//dirty code!!
void	Parser::copyServerBlock(std::ifstream &configStream, std::string &line, std::string &configContentString)
{
	while (std::getline(configStream, line))
	{
		line.erase(0, line.find_first_not_of("\t\r\n "));
		line.erase(line.find_last_not_of("\t\r\n ") + 1);
		if (line.empty() || line[0] == '#')
			continue ;
		size_t openingBracketPos = line.find('{');
		size_t closingBracketPos = line.find('}');
		//if opening bracket is found and closing bracket is not found
		configContentString += line + "\n";
		if (openingBracketPos != std::string::npos && closingBracketPos == std::string::npos)
			copyServerBlock(configStream, line, configContentString);
		else if (openingBracketPos != std::string::npos && closingBracketPos != std::string::npos)
			continue ;
		if (closingBracketPos != std::string::npos)
			break ;
	}
}


// std::string	Parser::copyServerBlock(std::ifstream &configStream, std::string &line)
// {
// 	std::string	configContentString;
// 	int			indentLevel = 0;
// 	while (std::getline(configStream, line))
// 	{
// 		line.erase(0, line.find_first_not_of("\t\r\n "));
// 		line.erase(line.find_last_not_of("\t\r\n ") + 1);
// 		if (line.empty() || line[0] == '#')
// 			continue ;
// 		size_t openingBracketPos = line.find('{');
// 		size_t closingBracketPos = line.find('}');
// 		configContentString += line + "\n";
// 		//if opening bracket is found and closing bracket is not found, indentlevel++
// 		if (openingBracketPos != std::string::npos && closingBracketPos == std::string::npos)
// 			indentLevel++;
// 		//if opening braket is found and closing bracket is found include whole string
// 		else if (openingBracketPos != std::string::npos && closingBracketPos != std::string::npos)
// 			continue ;
// 		if (closingBracketPos != std::string::npos)
// 			indentLevel--;
// 		if 
// 	}
// 	if (indentLevel != 0)
// 		std::cout << "syntax error conf file" << std::endl;
// 	return configContentString;
// }

void	Parser::createConfigObjects()
{
	std::ifstream configStream(configFilePath);
	if(!configStream.is_open() || !configStream.good())
		exit(1);

	std::string line;
	while(std::getline(configStream, line))
	{
		line.erase(0, line.find_first_not_of("\t\r\n "));
		line.erase(line.find_last_not_of("\t\r\n ") + 1);
		if (line.empty() || line[0] == '#')
			continue ;
		if (line.find("server") == 0)
		{
			std::string configContentString;
			copyServerBlock(configStream, line, configContentString);
			configContentString.erase(configContentString.find_last_of('}'));
			std::cout << "new config: \n" << configContentString << std::endl;
		}
	}
}

// void	Parser::getConfigFileString()
// {
// 	std::ifstream configFileStream(configFilePath);
// 	if (!configFileStream.is_open())
// 		exit(1);
// 	std::ostringstream ss;
// 	ss << configFileStream.rdbuf();
// 	std::cout << ss.str() << std::endl;
// 	configFileString = ss.str();
// }

std::vector<Config>	Parser::getConfigVector() const
{
	return configVector;
}