#include "../inc/Parser.hpp"
#include "../inc/Error.hpp"

Parser::Parser(char *configPath) : configFilePath(configPath) {createConfigObjects();}

Parser::~Parser() {}

//call Config constructor for every server block, put config object in the configVector
void	Parser::createConfigObjects()
{
	//check file for syntax errors, returns vector with all lines
	std::vector<std::string> configFileLines = formatFile();

	// printConfigLines(configFileLines);

	size_t begin = 0;
	size_t end = 0;
	size_t len = 0;
	for (int i = 0; i != configFileLines.size(); ++i)
	{
		//if server is found and and there is nothing behind "server"
		if (configFileLines[i].find("server") == 0 && configFileLines[i].length() == 6)
		{
			begin = i;
			len = lengthServerBlock(configFileLines, begin);
			end = begin + len;

			std::vector<std::string>	serverBlock(configFileLines.begin() + begin + 2, configFileLines.begin() + end);

			// std::cout << "\nserver block:" << std::endl;
			// printConfigLines(serverBlock);
			// std::cout << "\n";

			//adding server config to the config array 

			Config	newConfig(serverBlock);
			configVector.push_back(newConfig);
		}
		//if server is found and there is something behind server
		else if (configFileLines[i].find("server") != std::string::npos)
		{
			std::cout << "a" << std::endl;
			exitError(SYNTAX_ERROR);
		}
		//go to next server block
		i += len;
	}
	// check if config is empty if so exitError()
}

//performs a syntax check and throws an error if syntax is not good
//Will create a string without excess whitespace, comments or empty lines, each line will be followed by a '\n'
std::vector<std::string>	Parser::formatFile()
{
	std::ifstream configStream(configFilePath);
	if(!configStream.is_open() || !configStream.good())
		exit(1);

	std::vector<std::string>	configFile;
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

		configFile.push_back(line);
	}
	printConfigLines(configFile);
	checkBrackets(configFile);
	configStream.close();
	return (configFile);
}

int	Parser::lengthServerBlock(std::vector<std::string> configFileLines, int i)
{
	int begin = 0;
	int end = 0;
	int indentLevel = 0;
	std::string	line;

	begin = i;
	i++;
	//if there is no '{' after the server line or there is something behind the '{', syntax_error
	if (configFileLines[i].find("{") != 0 || configFileLines[i].length() != 1)
		exitError(SYNTAX_ERROR);
	for (; i != configFileLines.size(); i++)
	{
		line = configFileLines[i];
		for (int j = 0; line[j]; j++)
		{
			if (line[j] == '{')
				indentLevel++;
			else if (line[j] == '}')
				indentLevel--;
		}
		//if last '}' of serverBlock is found and there is nothing before or behind it serverBlock is completed
		if (indentLevel == 0 && (line.find("}")) == 0 && line.length() == 1)
			return (i - begin);
		else if (indentLevel == 0)
			exitError(SYNTAX_ERROR);
	}
	//no closing bracket found
	exitError(SYNTAX_ERROR);
	return (1);
}

//checks if there are to many opening or closing brackets
void	Parser::checkBrackets(std::vector<std::string> configFile)
{
	std::string							line;
	std::vector<std::string>::iterator	it;
	int									indentLevel = 0;

	for (it = configFile.begin(); it != configFile.end(); it++)
	{
		line = *it;
		for (int i = 0; line[i]; i++)
		{
			if (line[i] == '{')
				indentLevel++;
			else if (line[i] == '}')
				indentLevel--;
			//indentLevel should not be under 0 since that means there were more closing brackets then opening
			if (indentLevel < 0)
				exitError(SYNTAX_ERROR);
		}
	}
	if (indentLevel != 0)
		exitError(SYNTAX_ERROR);
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