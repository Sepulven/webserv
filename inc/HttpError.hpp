#pragma once 

#include <exception>

class HttpError : public std::exception
{
	public:
		const char *status;
		const char *msg;
		virtual const char *what() const throw();

		const char *get_msg(void) const;
		const char *get_status(void) const;
		HttpError(const char *, const char *);
};
