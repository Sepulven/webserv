#include <Req.hpp>

Req::Req()
{
}

Req::~Req()
{
}


int Req::is_complete(std::string &_data)
{
	std::string end = "\r\n\r\n";

	if (_data.find(end))
	{
		//parse the header to a map;
		if (method == "GET")
			return (1);
	}
	// Check for the end of the request, in the post scenario;
}
int Req::read(int fd)
{
	char buffer[1025];
	std::string& _data = &this->data;

	int bytes_read = read(fd, buffer, 1024);

	buffer[bytes_read] = '\0';
	if (bytes_read <= 0)
		return -1;
	
	while (bytes_read > 0)
	{
		_data += buffer;
		bytes_read = read(fd, buffer, 1024);
		buffer[bytes_read] = '\0';
	}
	return (is_complete(_data));
}

