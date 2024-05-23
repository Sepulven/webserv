#pragma once

#include <exception>
#include <iostream>
#include <string>

class ServerError : public std::exception
{
	private:
		const char *msg;
		virtual const char *what() const throw();
	public:
		const char * get_msg(void) const;
		ServerError(const char *_msg);
};
