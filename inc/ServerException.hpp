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

class InternalServerErrorException : public ServerException
{
	public:
		InternalServerErrorException() : ServerException("500 Internal server error") {}
};

#endif