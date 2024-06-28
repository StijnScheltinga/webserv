#include "../inc/Redirect.hpp"


Redirect::Redirect()
{
}

Redirect::Redirect(std::string url, int code) : url(url), code(code)
{
}

Redirect::~Redirect()
{
}

std::string Redirect::getUrl()
{
	return (this->url);
}

int Redirect::getCode()
{
	return (this->code);
}
