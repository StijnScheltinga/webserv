#ifndef ERROR_PAGE_HPP
# define ERROR_PAGE_HPP

# include <string>
# include <vector>

class ErrorPage
{
	private:
		std::string			path;
		std::vector<int>	statusCodes;

	public:
		ErrorPage(std::string ErrorPage);
		~ErrorPage();

		std::string	getPath();
		void		setPath(std::string path);
		void 		addStatusCode(std::string statusCode);
		std::vector<int> getStatusCodesVector();
		void		printErrorPage() const;

};

#endif