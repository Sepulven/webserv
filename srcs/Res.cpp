#include <Res.hpp>

Res::Res(ConnStream *_stream) : stream(_stream) {
	status["200"] = "OK";
	status["400"] = "BAD REQUEST";
	status["403"] = "FORBIDDEN";
	status["404"] = "NOT FOUND";

	content_type[".txt"] = "text/plain";
	content_type[".cpp"] = "text/plain";
	content_type[".hpp"] = "text/plain";
	content_type[".html"] = "text/html";
	content_type[".pdf"] = "application/pdf";
}

Res::~Res() { }

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

std::string	Res::get_response_body(void)
{
	std::string filename = FileManager::get_filename(stream->req->URL);

	// if (code != "200")
	// 	return (FileManager::read(filename));

	if (stream->req->method == "GET")
		return (FileManager::read(filename));
	if (stream->req->method == "POST")
		return ("File Uploaded Successfully!");
	if (stream->req->method == "DELETE")
		return ("File Deleted Successfully!");
	return ("The method is not even an option, how does it work?");
}

void	Res::build_response(std::string code)
{
	std::string ext = FileManager::get_extension(stream->req->URL);
	std::string content;

	this->code = code;

	content = get_response_body();
	this->data += "HTTP/1.1 " + code + " " + this->status[code] + "\r\n";
	this->data += "Content-Type: " + this->content_type[ext] + "\r\n";
	this->data += "Content-Length: " + content.length() + "\r\n";
	this->data += "\r\n\r\n";
	this->data += content;
}

void	Res::exec_delete(void)
{
	std::string path = stream->req->URL;

	//TODO: Check for permissions;

	if (path[0] == '/')
		path = path.substr(1);
	if (std::remove(path.c_str()) != 0)
		build_response("404");
	else
		build_response("200");
}

void Res::process_req(void)
{
	Req *req = stream->req;
	std::vector<std::string> methods;

	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");

	try
	{
		if (std::find(methods.begin(), methods.end(), req->method) == methods.end())
			send_response("400");
		if (req->method == "GET")
			send_file();
		else if (req->method == "POST")
			create_file();
		else if (req->method == "DELETE")
			exec_delete();
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}

	write(stream->fd, this->data.c_str(), this->data.length());
}
