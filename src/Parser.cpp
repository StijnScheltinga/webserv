#include "../inc/Parser.hpp"

Parser::Parser(char *configPath) : configFilePath(configPath) {createConfigObjects();}

Parser::~Parser() {}

//call Config constructor for every server block
//config class handles parsing of info inside Server block
//Put config object in the configVector

// void	Parser::copyServerBlock(std::istringstream &configStream, std::string &line, std::string &serverConfig)
// {
// 	while (std::getline(configStream, line))
// 	{
// 		size_t openingBracketPos = line.find('{');
// 		size_t closingBracketPos = line.find('}');
// 		//if opening bracket is found and closing bracket is not found
// 		serverConfig += line + "\n";
// 		if (openingBracketPos != std::string::npos && closingBracketPos == std::string::npos)
// 			copyServerBlock(configStream, line, serverConfig);
// 		else if (openingBracketPos != std::string::npos && closingBracketPos != std::string::npos)
// 			continue ;
// 		if (closingBracketPos != std::string::npos)
// 			break ;
// 	}
// }


//performs a syntax check and throws an error if syntax is not good
//Will create a string without excess whitespace, comments or empty lines.
//each line will be followed by a '\n'
//actual syntax check will be implemented later since this is not necessary for a working webserv
std::vector<std::string>	Parser::syntaxError()
{
	std::ifstream configStream(configFilePath);
	if(!configStream.is_open() || !configStream.good())
		exit(1);

	std::vector<std::string>	configFileLines;
	std::string line;
	while (std::getline(configStream, line))
	{
		//remove whitespace
		line.erase(0, line.find_first_not_of("\t\r\n "));
		line.erase(line.find_last_not_of("\t\r\n ") + 1);
		
		//remove comments
		size_t commentPos = line.find('#');
		if (commentPos != std::string::npos)
			line = line.substr(0, commentPos);

		if (line.empty())
			continue ;

		configFileLines.push_back(line);
	}
	configStream.close();
	return (configFileLines);
}

void	Parser::createConfigObjects()
{
	//check file for syntax errors, returns vector with all lines
	std::vector<std::string> configFileLines = syntaxError();

	// printConfigLines(configFileLines);

	size_t begin = 0;
	size_t end = 0;
	size_t len = 0;
	for (int i = 0; i != configFileLines.size(); ++i)
	{
		if (configFileLines[i].find("server") == 0)
		{
			begin = i;
			len = lengthServerBlock(configFileLines, begin);
			end = begin + len;

			std::vector<std::string>	serverBlock(configFileLines.begin() + begin, configFileLines.begin() + end);

			//adding server config to the config array 
			Config	newConfig(serverBlock);
			configVector.push_back(newConfig);
		}
		i += len;
	}
}

int	Parser::lengthServerBlock(std::vector<std::string> configFileLines, int i)
{
	int	lengthServerBlock = 0;
	int	indentLevel = 0;
	for (; i != configFileLines.size(); i++)
	{
 		size_t openingBracketPos = configFileLines[i].find('{');
		size_t closingBracketPos = configFileLines[i].find('}');
		if (openingBracketPos != std::string::npos)
			indentLevel++;
		if (closingBracketPos != std::string::npos)
			indentLevel--;
		if (indentLevel == 0 && lengthServerBlock != 0)
			break;
		lengthServerBlock++;
	}
	return lengthServerBlock;
}

void	Parser::printConfigLines(std::vector<std::string> configLines)
{
	for (int i = 0; i != configLines.size(); i++)
		std::cout << configLines[i] << std::endl;
}

std::vector<Config>	Parser::getConfigVector()
{
	return configVector;
}