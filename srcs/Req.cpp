#include <Req.hpp>

Req::Req(ConnStream * _stream) : stream(_stream)
{
}

Req::~Req()
{
}


/*
	* Log the response on sthe stdout;
*/
void Req::log(void) const {
		std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
		std::cout << file_path << std::endl
				<< filename << std::endl
				<< file_ext << std::endl
				<< query_string << std::endl
				;

		std::cout << "********************************" << std::endl;
		std::cout << data;
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

	for (size_t j = 1; j < length; j++)
	{
		line = header[j];

		i = line.find(":");
		key = line.substr(0, i);
		value = line.substr(i+1);
		this->header[key] = value;
	}
}

/*
	TODO: Fix edgy cases of the extension ex.: .a ; a. ; a..a ; a..
	* Parsing on the URL;
*/
void	Req::set_URL_data(std::string& URL)
{
	std::vector<std::string> vec = split(URL, "?");
	struct stat fileStat;
	size_t pos;

	file_path = vec[0].substr(1); // * Removes the first slash

	query_string = URL.substr(file_path.length() + 1);

	pos = file_path.find_last_of('/');
	if (pos != std::string::npos)
		filename = file_path;
	else
		filename = file_path.substr(pos + 1);

	// * Sets the file_type - related to how the request is going to get treated
	if (stat(file_path.c_str(), &fileStat) < -1)
		path_type = _NONE;
	if (S_ISDIR(fileStat.st_mode))
		path_type = _DIRECTORY;
	else if (S_ISREG(fileStat.st_mode))
		path_type = _FILE;
	else
		path_type = _NONE;

	// * Sets the file extension, if it is a file
	pos = filename.find_last_of('.');
	if (path_type == _FILE && pos == std::string::npos)
		file_ext = ".txt";
	else if (path_type == _FILE)
		file_ext = filename.substr(pos);
	else
		file_ext = "";

}

void	Req::parser(void)
{

	std::vector<std::string> request = split(this->data, "\r\n\r\n");
	std::vector<std::string> message_header = split(request[0], "\r\n");

	this->body = request[1];

	this->request_line = message_header[0];

	std::vector<std::string> request_line_tokens = split(message_header[0], " ");

	this->method = request_line_tokens[0];
	this->URL = request_line_tokens[1];
	this->http = request_line_tokens[2];

	this->set_URL_data(this->URL);
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

	if (!_data.find("/r/n/r/n"))
		return (0);
	this->parser();
	return (1);
}

