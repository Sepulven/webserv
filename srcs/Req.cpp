#include <Req.hpp>

Req::Req(ConnStream * _stream) : stream(_stream)
{
	this->cgi_path = "a.py";
}

Req::~Req()
{
}

static void print_uint(const std::basic_string<uint8_t> &str)
{
	std::basic_string<uint8_t>::const_iterator it = str.begin();
	std::basic_string<uint8_t>::const_iterator ite = str.end();

	std::cout << "This is some binaries: " << std::endl << std::endl;
	for (; it != ite; it++)
		std::cout << static_cast<unsigned char>(*it);
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

		print_uint(data);
		std::cout << "********************************" << std::endl;
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

	if (file_path.length() == 0)
		file_path = ".";

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

void	Req::set_raw_body(size_t pos)
{
	size_t length = this->data.length();
	std::basic_string<uint8_t> data = this->data;

	for (size_t i = pos; i < length; i++)
	{
		std::cout << static_cast<unsigned char>(data[i]) << std::endl;
		this->raw_body.push_back(static_cast<int>(data[i]));
	}

}

void	Req::parser(void)
{
	std::string _data(this->data.begin(), this->data.end());
	size_t end_header_pos = _data.find("\r\n\r\n");
	std::vector<std::string> request = split(_data, "\r\n\r\n");
	std::vector<std::string> message_header = split(request[0], "\r\n");


	print_uint(this->data);

	this->request_line = message_header[0];

	std::vector<std::string> request_line_tokens = split(message_header[0], " ");

	std::cout << "Message Header: " << message_header[0] << std::endl;

	this->method = request_line_tokens[0];
	this->URL = request_line_tokens[1];
	this->http = request_line_tokens[2];

	this->set_URL_data(this->URL);
	this->set_header(message_header);

	this->content_length = std::atoi(this->header["Content-length"].c_str());
	if (end_header_pos != std::string::npos)
		this->raw_body.append(this->data.begin() + end_header_pos + 4, this->data.end());
	std::cout << "This is the content length: " << content_length << std::endl
			  << "This is the body length: " << this->raw_body.length() << std::endl;
}

/*
 * Refactor it for the chunked;
 * Refactor based on the bad connections;
*/
int Req::read(int fd)
{
	uint8_t buffer[4096 + 1];
	std::basic_string<uint8_t>& _data = this->data;
	const uint8_t crlf[] = {'\r', '\n', '\r', '\n'};
	std::basic_string<uint8_t> pattern(crlf, 4);

	int bytes_read = ::read(fd, buffer, 4096);

	if (bytes_read <= 0) // * Closes the connection
		return -1;

	while (bytes_read > 0)
	{
		_data.append(buffer, buffer + bytes_read);
		if (method == "" && _data.find(pattern) != std::string::npos)
			this->parser();
		else if (method != "")
			raw_body.append(buffer, buffer + bytes_read);
		bytes_read = ::read(fd, buffer, 4096);
	}
	if (_data.find(pattern) != std::string::npos && raw_body.length() >= this->content_length)
		return (1);
	return (0);
}

