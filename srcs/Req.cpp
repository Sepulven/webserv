#include <Req.hpp>

Req::Req(ConnStream * _stream) : stream(_stream), out_of_bound(std::string::npos)
{
	this->cgi_path = "a.py";
	this->chunk_length = -1;
}

Req::~Req() {}

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

	RawData::print_uint(data);
	std::cout << "********************************" << std::endl;
}

/*
 TODO: Protecy in case of invalid syntax Throw error;
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
	std::vector<std::string> vec = RawData::split(URL, "?");
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

/*
 * Returns void
*/
void	Req::parser(void)
{
	size_t end_header_pos = RawData::find(data, "\r\n\r\n");
	std::vector<uint8_t> __header  = RawData::substr(data, 0, end_header_pos);
	std::string request(__header.begin(), __header.end());
	std::vector<std::string> message_header = RawData::split(request, "\r\n");

	this->request_line = message_header[0];

	std::vector<std::string> request_line_tokens = RawData::split(message_header[0], " ");

	this->method = request_line_tokens[0];
	this->URL = request_line_tokens[1];
	this->http = request_line_tokens[2];

	this->set_URL_data(this->URL);
	this->set_header(message_header);

	RawData::print_uint(data);

	if (header["Content-Length"] != "")
		this->content_length = std::atoi(&this->header["Content-Length"].c_str()[1]);
	std::vector<uint8_t> sub_vec = RawData::substr(data, end_header_pos + 4, data.size() - end_header_pos - 4);
	if (header["Transfer-Encoding"] == " chunked") //* there is space here, we need to get rid of it
		this->unchunk(sub_vec.data(), sub_vec.size());
	else if (end_header_pos != out_of_bound)
		RawData::append(raw_body, sub_vec);
}

/*
 * Returns nothing;
 * Sets all of the previous chunk to the raw body
 * 
 * 5\r\nbanan\r\n
 TODO: Add conditional that checks whether the difference between right and left are bigger than the size;
*/
void Req::get_last_chunk(const std::vector<uint8_t>& buff)
{
	std::size_t left_crlf = RawData::find(buff, "\r\n");
	std::size_t right_crlf = RawData::find(buff, "\r\n", left_crlf + 2);
	std::string length(buff.begin(), buff.begin() + 2);
	std::vector<uint8_t> chunk_content;
	std::stringstream ss;


	ss << std::hex << length.c_str();
	ss >> chunk_length;

	std::cout << "start of the body --" << std::endl;
	RawData::print_uint(buff);
	std::cout << "end of the body --" << std::endl;

	int i = 0;
	while (right_crlf != out_of_bound && chunk_length > 0)
	{
		std::cout << "["<< i++ << "] chunk_length:decimal=" << chunk_length << " -> chunk_length:hex=" << length << std::endl;
		std::cout << "crlf_pos=" << left_crlf << std::endl;

		chunk_content = RawData::substr(buff, left_crlf + 2, chunk_length);

		RawData::append(chunk, chunk_content);

		left_crlf = RawData::find(buff, "\r\n", left_crlf + 2);
		length == std::string(buff.begin(), buff.begin() + left_crlf);
		ss << std::hex << length.c_str();
		ss >> chunk_length;
	}
}

/*
 * Returns nothing.
 * Sets the chunk_length;
 * Set the chunk until it reaches it's chunk_length;
 * Write to the raw_body;
*/
void	Req::unchunk(const uint8_t *_buff, size_t _length)
{
	std::vector<uint8_t> buff(_buff, _buff + _length);
	std::size_t crlf_pos = RawData::find(buff, "\r\n");

	if (chunk_length == -1 && crlf_pos != out_of_bound)
		this->get_last_chunk(buff);
	else if (chunk_length == -1 && chunk_length < (int)chunk.size())
		RawData::append(chunk, buff); //* Questionable needs to check it more precisely;
	else if (chunk_length == (int)chunk.size()) // * Sets to the raw_body
	{
		RawData::append(raw_body, chunk);
		chunk.clear();
		chunk_length = -1 * (chunk_length != 0);  // * Restarts the circle
	}
}

/*
 * Returns 0 in case the read hasn't been finished;
 * Returns 1 when the whole request has been read;
 * Returns -1 to close the connection
*/
int Req::read(int fd)
{
	uint8_t buffer[4096 + 1];
	int bytes_read = ::read(fd, buffer, 4096);

	if (bytes_read <= 0)
		return -1;
	while (bytes_read > 0)
	{
		RawData::append(data, buffer, bytes_read);
		if (method == "" && RawData::find(data, "\r\n\r\n") != out_of_bound)
			this->parser();
		else if (method != "" && header["Transfer-Encoding"] == " chunked")
			this->unchunk(buffer, bytes_read);
		else if (method != "")
			RawData::append(raw_body, buffer, bytes_read);
		bytes_read = ::read(fd, buffer, 4096);
	}
	if (RawData::find(data, "\r\n\r\n") != out_of_bound && raw_body.size() >= content_length)
		return (1);
	if (header["Transfer-Encoding"] == " chunked" && chunk_length == 0)
		return (1);
	return (0);
}

