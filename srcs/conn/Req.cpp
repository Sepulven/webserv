#include <Req.hpp>

Req::Req(ConnStream * _stream) : stream(_stream), out_of_bound(std::string::npos)
{
	this->cgi_path.push_back("a.py");
	this->cgi_path.push_back("a.php");

	this->content_length = 0;
}

Req::~Req() {}

/*
 * Gets the values from header nad set them in the map;
 * Validates the headers, checks if they are in key-value pairs;
*/
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

		i = line.find(": ");
		if (i == std::string::npos || line.length() + 1 < i) // * Out of bound;
			throw HttpError("400", "Invalid Request");
		key = line.substr(0, i);
		value = line.substr(i+1);
		this->header[key] = value;
	}
}

/*
 * Set file path;
 * Don't throw an error;
*/
void	Req::set_path_type(void)
{
	struct stat fileStat;

	// * Sets the file_type - related to how the request is going to get treated
	if (stat(file_path.c_str(), &fileStat) < -1)
		path_type = _NONE;
	if (S_ISDIR(fileStat.st_mode))
		path_type = _DIRECTORY;
	else if (S_ISREG(fileStat.st_mode))
		path_type = _FILE;
	else
		path_type = _NONE;
}

/*
 * Sets the file extension, if it is a file
*/
void	Req::set_file_ext(void)
{
	size_t pos;

	pos = filename.find_last_of('.');
	if (path_type == _FILE && pos == std::string::npos
	 && pos >= filename.length())
		file_ext = ".txt";
	else if (path_type == _FILE)
		file_ext = filename.substr(pos);
	else // * It is a directory or none;
		file_ext = "";
}

/*
 * Parsing on the URL;
*/
void	Req::set_URL_data(std::string& URL)
{
	std::vector<std::string> vec = RawData::split(URL, "?");
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

	set_path_type();
	set_file_ext();
}

void Req::set_content_length(void)
{

	if (header["Content-Length"] != "")
		content_length = std::atoi(header["Content-Length"].c_str());
	if (header["Content-Length"] != "" && content_length > (size_t)stream->server->max_cb_size)
		throw HttpError("413", "Payload Too Large");
}

/*
 * Parses the request;
*/
void	Req::parser(void)
{
	size_t end_header_pos = RawData::find(data, "\r\n\r\n");
	std::vector<uint8_t> __header  = RawData::substr(data, 0, end_header_pos);
	std::string request(__header.begin(), __header.end());
	std::vector<uint8_t> sub_vec;

	std::vector<std::string> message_header = RawData::split(request, "\r\n");

	this->request_line = message_header[0];

	std::vector<std::string> request_line_tokens = RawData::split(message_header[0], " ");

	this->method = request_line_tokens[0];
	this->URL = request_line_tokens[1];
	this->http = request_line_tokens[2];

	this->set_URL_data(this->URL);
	this->set_header(message_header);



	this->set_content_length();

	sub_vec = RawData::substr(data, end_header_pos + 4, data.size() - end_header_pos - 4);
	if (end_header_pos != out_of_bound)
		RawData::append(raw_body, sub_vec);
}

/*
 * Returns 0 in case the read hasn't been finished;
 * Returns 1 when the whole request has been read;
 * Returns -1 to close the connection;

 * If an error is found, sets the response's status
 * and finishes the request;
*/
int Req::read(int fd)
{
	uint8_t buffer[4096 + 1];
	int bytes_read = ::read(fd, buffer, 4096);

	if (bytes_read <= 0)
		return -1;
	try
	{
		while (bytes_read > 0)
		{
			RawData::append(data, buffer, bytes_read);
			if (method.empty() && RawData::find(data, "\r\n\r\n") != out_of_bound)
				this->parser();
			else if (!method.empty())
				RawData::append(raw_body, buffer, bytes_read);
			bytes_read = ::read(fd, buffer, 4096);
		}
		if (RawData::find(data, "\r\n\r\n") != out_of_bound 
			&& raw_body.size() >= content_length)
			return (1);
	}
	catch (const HttpError &e)
	{
		stream->res->status_code = e.get_status();
		stream->res->error_msg = e.get_msg();
		return (1);
	}
	return (0);
}
