#ifndef REDIRECT_HPP
# define REDIRECT_HPP

#include <iostream>

class Redirect
{
	private:
		std::string url;
		int code;
	
	public:
		Redirect();
		Redirect(std::string url, int code);
		~Redirect();
		std::string getUrl();
		int getCode();

};

#endif