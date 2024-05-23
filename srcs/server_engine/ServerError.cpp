#include <ServerError.hpp>

ServerError::ServerError(const char *_msg) : msg(_msg) {}


const char *ServerError::get_msg(void) const
{
	return (msg);
}
const char *ServerError::ServerError::what() const throw()
{
	return (msg);
}
