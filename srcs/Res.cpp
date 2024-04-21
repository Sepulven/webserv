#include <Res.hpp>

Res::Res(ConnStream *_stream) : stream(_stream)
{
}

Res::~Res()
{
}

int Res::send(void) const
{
	const char *httpResponse = "HTTP/1.1 200 OK\r\n"
							   "Content-Type: text/plain\r\n"
							    "Content-Length: 8\r\n"
							   "\r\n"
							   "Success!"
							   "\r\n";
	std::string response(httpResponse);

	if (write(stream->fd, response.c_str(), response.size()) < 0)
		return (-1);
	return (1);
}
