#ifndef SERVEREXCEPTION_HPP
# define SERVEREXCEPTION_HPP

class ServerException : public std::exception
{
	public:
		ServerException(const std::string &message) : _message(message) {}
		virtual const char *what() const noexcept override
		{
			return _message.c_str();
		}

	private:
		std::string _message;
};

class BadRequestException : public ServerException
{
    public:
		BadRequestException() : ServerException("400 Bad request") {}

};

class ForbiddenException : public ServerException
{
	public:
		ForbiddenException() : ServerException("403 Forbidden") {}
};

class NotFoundException : public ServerException
{
	public:
		NotFoundException() : ServerException("404 Page not found") {}
};

class MethodNotAllowedException : public ServerException
{
	public:
		MethodNotAllowedException() : ServerException("405 Method not allowed") {}
};

class ContentTooLargeException : public ServerException
{
	public:
		ContentTooLargeException() : ServerException("413 Content too large") {}
};

class InternalServerErrorException : public ServerException
{
	public:
		InternalServerErrorException() : ServerException("500 Internal server error") {}
};

class RedirectionException : public ServerException
{
	public:
		RedirectionException(const std::string &url, int code) : ServerException("301 Moved Permanently"), _url(url), _code(code) {}
		std::string getUrl() const { return _url; }
		int getCode() const { return _code; }

	private:
		std::string _url;
		int _code;
};


#endif