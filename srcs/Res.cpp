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
	* Log the response on sthe stdout;
*/
void Res::log(void) const {
	std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
	std::cout << this->data;
}

/*
	* Must check for the permissions before executing;
	* Must handle in case of the URL is a a directory;
*/
int Res::send(void)
{
	Req *req = stream->req;
	std::vector<std::string> methods;
	std::stringstream ss;	

	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");

	// * We are going to check for permission before doing anything
	if (req->method == "GET")
		exec_get();
	else if (req->method == "POST")
		exec_post();
	else if (req->method == "DELETE")
		exec_delete();

	ss << "HTTP/1.1 " << code << " " << this->status[code] << "\r\n";
	ss << "Content-Type: " << content_type[stream->req->file_ext] <<  "\r\n";
	ss << "Content-Length: " << content.length() << "\r\n";
	ss << "\r\n\r\n";
	ss << content;
	ss << "\r\n\r\n";

	this->data = ss.str();
	return (write(stream->fd, this->data.c_str(), this->data.length()));
}

void	Res::exec_delete(void)
{
	std::string path = stream->req->file_path;

	if (path[0] == '/')
		path = path.substr(1);
	if (std::remove(path.c_str()) != 0)
	{
		this->content = "We couldn't find the file to be deleted!";		
		this->code = "404";
	}
	else
	{
		this->content = "We've deleted the file succesfully!";
		this->code = "200";
	}
}

void	Res::exec_get(void)
{
	if (stream->req->path_type == _FILE)
	{
		this->content = FileManager::read_file(stream->req->file_path);
		this->code = "200";
	}
	if (stream->req->path_type == _DIRECTORY)
	{
		this->content = FileManager::directory_listing(stream->req->file_path);
		this->code = "200";
	}
	if (stream->req->path_type == _NONE)
	{
		std::cout << "Here we are" << std::endl;
		this->content = FileManager::read_file("errors/404.html");
		this->code = "404";
	}
}

void	Res::exec_post(void)
{
	this->code = "200";
	this->content = FileManager::create_file(stream->req->filename, stream->req->body);
}

