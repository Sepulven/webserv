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


/*
	* Must check for the permissions before executing;
	* Must handle in case of the URL is a a directory;
*/
int Res::send(void)
{
	Req *req = stream->req;
	std::vector<std::string> methods;

	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");

	// std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
	// std::cout << stream->req->file_path << std::endl
	// 		  << stream->req->filename << std::endl
	// 		  << stream->req->file_ext << std::endl
	// 		  << stream->req->query_string << std::endl
	// 		  ;

	// std::cout << "********************************" << std::endl;
	// std::cout << stream->req->data;
	try
	{
		if (std::find(methods.begin(), methods.end(), req->method) == methods.end())
			build_response("400");
		else if (req->method == "GET")
			exec_get();
		else if (req->method == "POST")
			exec_post();
		else if (req->method == "DELETE")
			exec_delete();
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}

	// std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
	// std::cout << this->data;
	return (write(stream->fd, this->data.c_str(), this->data.length()));
}

std::string	Res::get_response_body(void)
{
	std::string path = stream->req->file_path;

	// if (code != "200")
	// 	return (FileManager::read(filename));

	if (stream->req->method == "GET")
		return (FileManager::read_file(path));
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
	std::stringstream ss;

	this->code = code;

	content = get_response_body();

	ss << content.length();

	this->data += "HTTP/1.1 " + code + " " + this->status[code] + "\r\n";
	this->data += "Content-Type: " + this->content_type[ext] + "\r\n";
	this->data += "Content-Length: " + ss.str()  + "\r\n";
	this->data += "\r\n\r\n";
	this->data += content;
	this->data += "\r\n\r\n";
}

void	Res::exec_delete(void)
{
	std::string path = stream->req->URL;

	if (path[0] == '/')
		path = path.substr(1);
	if (std::remove(path.c_str()) != 0)
		build_response("404");
	else
		build_response("200");
}

void	Res::exec_get(void)
{
	std::string URL = stream->req->URL;

	if (URL[0] == '/')
		URL = "." + URL;
	int fd = open(URL.c_str(), O_RDONLY);
	if (fd == -1)
	{
		build_response("404");
		return ;
	}
	close(fd);
	if (opendir(URL.c_str()))
	{
		(void)URL;
		// directory_listing();
	}
	else
	{
		if (open(URL.c_str(), O_RDONLY) == -1)
			build_response("404");
		build_response("200");
	}
}

void	Res::exec_post(void)
{
	// Cheks for permission;
	std::string filename = FileManager::get_filename(stream->req->URL);

	build_response(FileManager::create_file(filename, stream->req->body));
}

