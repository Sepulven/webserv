#include <Res.hpp>

Res::Res(ConnStream *_stream) : stream(_stream) { }

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

void Res::send_response(std::string code)
{
	(void)code;
	// std::string status;
	// std::string extension = "";

	// std::string content = get_response_body(code);

	// if (code == "200")
	// {
	// 	status = "OK";
	// 	if (method == "GET")
	// 		extension = this->get_extension(this->file_to_open);
	// 	else if (method == "POST")
	// 		extension = this->get_extension(this->filename);
	// 	else if (method == "DELETE")
	// 		extension = this->get_extension(this->location);
	// }
	// else
	// {
	// 	if (code == "400")
	// 		status = "BAD REQUEST";
	// 	else if (code == "403")
	// 		status = "FORBIDDEN";
	// 	else if (code == "404")
	// 		status = "NOT FOUND";
	// 	// extension = ".html";
	// 	extension = this->get_extension(this->error_pages[code]);
	// }

	// static std::map<std::string, std::string> content_type;
	// content_type[".txt"] = "text/plain";
	// content_type[".cpp"] = "text/plain";
	// content_type[".hpp"] = "text/plain";
	// content_type[".html"] = "text/html";
	// content_type[".pdf"] = "application/pdf";

	// std::string response = "HTTP/1.1 " + code + " " + status + "\r\nContent-Type: " + content_type[extension] + "\r\nContent-Length: " + intToString(content.length()) + "\r\n\r\n" + content;
	// std::cout << ">>>>>>>>>>>>>>>\n"
	// 		  << response << std::endl;

	// write(stream->fd, response.c_str(), response.size());

	// if (code != "200")
	// 	throw std::invalid_argument("");
}

void Res::send_file(void)
{
	std::string path;
	DIR *dir;

	// Check if the req. is allowed or not for this route;

	// Concactenate the ., me must check from the config file
	if (stream->req->URL[0] == '/')
		path = "." + stream->req->URL[0];

	if (open(path.c_str(), O_RDONLY) == -1)
		this->send_response("404");

	dir = opendir(path.c_str());
	if (dir != NULL) // directory
	{
		// If there is a index file defined in the config, sends it;
		if ("")
		{
			if (open(path.c_str(), O_RDONLY) == -1)
				this->send_response("404");
			this->send_response("200");
		}
		// If directory listing is active do it;
		else if (false) //this->autoindex
			this->directory_listing();
		// Doesn't have permission;
		else
			this->send_response("403");
	}
	// send file;
	else
	{
		if (open(path.c_str(), O_RDONLY) == -1)
			this->send_response("404");
		this->send_response("200");
	}
}

void Res::create_file(void)
{
	// if (!this->post_allowed)
	// 	this->send_response("400");

	// std::ofstream file; // check if files exists
	// file.open(this->filename.c_str(), std::ios::in | std::ios::binary);
	// if (file.is_open())
	// 	this->send_response("400");

	// std::ofstream c_file(this->filename.c_str()); // create file
	// if (!c_file)
	// 	this->send_response("400");

	// c_file << this->body; // populate file

	this->send_response("200");
}

void Res::delete_file(void)
{

	// if (!this->delete_allowed)
	// 	this->send_response("400");

	// if (this->location[0] == '/')
	// 	location = location.substr(1);

	// if (std::remove(location.c_str()) != 0)
	// 	send_response("404");

	send_response("200");
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
			delete_file();
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}
