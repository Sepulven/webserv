#include <HttpError.hpp>

/*Exception class*/
HttpError::HttpError(const char *_status, const char *_msg) : status(_status), msg(_msg) {}

const char *HttpError::get_msg(void) const
{
	return (msg);
}

const char *HttpError::get_status(void) const
{
	return (status);
}

const char *HttpError::what() const throw()
{
	return (msg);
}
