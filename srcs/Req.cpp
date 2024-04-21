#include <Req.hpp>

Req::Req(ConnStream * _stream) : stream(_stream)
{
}

Req::~Req()
{
}

static std::vector<std::string> split(const std::string& base, const std::string& delimiter)
{
	std::istringstream iss(base);
	std::string token;
	std::vector<std::string> tokens;
	size_t startPos = 0;

	size_t pos = base.find(delimiter, startPos);
	while (pos != std::string::npos)
	{
		token = base.substr(startPos, pos - startPos); 
		tokens.push_back(token);

		startPos = pos + delimiter.length();
		pos = base.find(delimiter, startPos);
	}
	tokens.push_back(base.substr(startPos));
	return tokens;
}


void	Req::set_header(std::vector<std::string>& header)
{
	std::string	line;
	std::size_t	i = 0;
	std::string	key;
	std::string	value;
	
	std::size_t length = header.size();

	std::cout << "here we are " << std::endl;
	for (size_t j = 1; j < length; j++)
	{
		line = header[j];

		i = line.find(":");
		key = line.substr(0, i);
		value = line.substr(i+1);
		this->header[key] = value;
	}
}

void	Req::parser(void)
{

	std::vector<std::string> request = split(this->data, "\n\r");
	std::vector<std::string> message_header = split(request[0], "\n");

	this->body = request[1];

	this->request_line = message_header[0];

	std::vector<std::string> request_line_tokens = split(message_header[0], " ");

	this->method = request_line_tokens[0];
	this->http = request_line_tokens[1];
	this->URL = request_line_tokens[2];

	this->set_header(message_header);
}

int Req::read(int fd)
{
	char buffer[4096 + 1];
	std::string& _data = this->data;

	int bytes_read = ::read(fd, buffer, 4096);

	buffer[bytes_read] = '\0';
	if (bytes_read <= 0)
		return -1;

	while (bytes_read > 0)
	{
		_data += buffer;
		bytes_read = ::read(fd, buffer, 4096);
		buffer[bytes_read] = '\0';
	}
	if (_data.find("/r/n/r/n"))
	{
		this->parser();
		return (1);
	}
	return (0);
}

