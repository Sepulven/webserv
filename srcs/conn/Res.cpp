#include <Res.hpp>

/*
 * We must have this map globally;
 ! Performance issue with error handling issue;
*/
Res::Res(ConnStream *_stream) : stream(_stream)
{
	// * Error maps;
	status["200"] = "OK";
	status["201"] = "CREATED";
	status["400"] = "BAD REQUEST";
	status["403"] = "FORBIDDEN";
	status["404"] = "NOT FOUND";

	// * Content type definition
	content_type[".txt"] = "text/plain";
	content_type[".cpp"] = "text/plain";
	content_type[".hpp"] = "text/plain";
	content_type[".html"] = "text/html";
	content_type[".pdf"] = "application/pdf";


	// * In case there is no extension;
	std::string &file_ext = stream->req->file_ext;
	file_ext = file_ext == "" ? ".html" : file_ext;
}

Res::~Res() {}

/*
 * In case of success builds the response;
 * In case of error gets the error page and loads it;
 * If the error page is not found builds a new one;
*/
int Res::build_http_response(void)
{
	std::map<int, std::string> error_pages = stream->server->error_pages;
	Req *req = stream->req;
	std::stringstream ss;

	if (status_code[0] != '2') // * There is an error;
		this->content = FileManager::build_error_pages(error_pages, status_code, error_msg);
	ss << "HTTP/1.1 " << status_code << " " << this->status[status_code] << "\r\n";
	ss << "Content-Type: " << content_type[req->file_ext] << "\r\n";
	ss << "Content-Length: " << content.length() << "\r\n\r\n";
	ss << content;

	this->data = ss.str();
	return (write(stream->fd, this->data.c_str(), this->data.length()));
}

/*
 * Must check for the permissions before executing;
 * Execute the action of each individual method;
 * In case of error during the execution, changes the state of the response;
 */

bool Res::validate_route_name(std::string name, std::string filePath) {
	if (name == filePath)
		return true;
	size_t i = filePath.find_first_of('/');
	if (i != std::string::npos)
		if (std::strncmp(name.c_str(), filePath.c_str(), name.size()) == 0)
			return true;
	return false;
}

void	Res::expand_file_path(void)
{
	Req *req = stream->req;

	std::cout << "==========AFTER CHECK==========" << std::endl;
	for (size_t i = 0; i < this->stream->server->routes.size(); i++)  // * For debug purposes
		std::cout << "name: " << this->stream->server->routes[i].name << " | file_path: " << req->file_path << std::endl;

	for (size_t i = 0; i < this->stream->server->routes.size(); i++) {
		if (validate_route_name(this->stream->server->routes[i].name, req->file_path))
		{
			std::cout << "DEBUG" << std::endl;
			size_t j = 0;
			while (j <= req->file_path.size() && this->stream->server->routes[i].name[j] == req->file_path[j])
				j++;
			j--;
			if (!req->file_path[j])
				req->file_path = this->stream->server->routes[i].root.substr(1);
			else
				req->file_path = this->stream->server->routes[i].root.substr(1) + req->file_path.substr(j);
			if (!this->stream->server->routes[i].index.empty()) {
				req->file_path = req->file_path + "/" + this->stream->server->routes[i].index.back();
				req->path_type = _FILE;
			}
			std::cout << "new file_path: " << req->file_path << std::endl;
			// expand file path (replace name for root: /gatos/Req.cpp -> srcs/conn/Req.cpp)
		}
	}
}

int Res::send(void)
{
	Req *req = stream->req;

	std::vector<std::string> &cgi_path = req->cgi_path;
	std::vector<std::string>::iterator it = std::find(cgi_path.begin(), cgi_path.end(), req->file_path);

	if (!this->status_code.empty() && !this->error_msg.empty())
		return (build_http_response());

	std::cout << "File_path: " << req->file_path << "$" << std::endl;
	if (req->file_path.find('.') == std::string::npos || (req->file_path.size() == 1 && req->file_path[0] == '.')) // TODO: Ememndar gambiarra usando URL_DATA (req->path_type == _DIRECTORY)
	expand_file_path();
	// else {
	// 	std::cout <<  "ERRO FDD\n";
	// 	this->content = FileManager::read_file("error/403.html"); // change for error page variable
	// 	this->add_ext = ".html";
	// 	this->status_code = "403";
	// 	return (build_http_response());
	// }
	try
	{
		if (it != req->cgi_path.end())
			return (this->exec_CGI());
		if (req->method == "GET")
			exec_get();
		else if (req->method == "POST")
			exec_post();
		else if (req->method == "DELETE")
			exec_delete();
	}
	catch (const HttpError &e)
	{
		this->error_msg = e.get_msg();
		this->status_code = e.get_status();
	}
	return (build_http_response());
}


/*
TODO: Shrink the size of the function;
*/
int Res::exec_CGI(void)
{
	Req *req = stream->req;
	std::string raw_body(req->raw_body.begin(), req->raw_body.end());
	std::string data(req->data.begin(), req->data.end());
	int pipe_fd[2];
	int pipe_fd_aux[2];

	char *argv0;

	if (req->file_ext == ".py")
		argv0 = const_cast<char *>("/usr/bin/python3");
	else if (req->file_ext == ".php")
		argv0 = const_cast<char *>("/usr/bin/php");
	else
		argv0 = const_cast<char *>("");

	char *argv1 = const_cast<char *>(req->file_path.c_str());
	char *const argv[] = {argv0, argv1, NULL};

	pipe(pipe_fd);
	pipe(pipe_fd_aux);
	pid_t pid = fork();

	if (pid == 0)
	{
		std::vector<std::string> request;
		request.push_back("request=" + data);
		request.push_back("path=" + req->file_path);
		request.push_back("method=" + req->method);
		request.push_back("body=" + raw_body);
		request.push_back("content-type=" + content_type[req->file_ext]);
		// request.push_back("content-lenght=" + req->body.length());

		char **envp = new char *[request.size() + 1];
		size_t i = 0;
		for (i = 0; i < request.size(); i++)
		{
			envp[i] = new char[request[i].size() + 1];
			std::strcpy((char *)envp[i], request[i].c_str());
		}
		envp[i] = NULL;

		close(pipe_fd[0]); // Close read end
		dup2(stream->fd, STDOUT_FILENO); // Redirect stdout to the write end

		close(pipe_fd_aux[1]); // Close write end
		dup2(pipe_fd_aux[0], STDIN_FILENO); // Redirect stdin to the read end

		int dev_null = open("/dev/null", O_WRONLY);
		dup2(dev_null, STDERR_FILENO); // redirecting stderr to /dev/null
		close(dev_null);

		execve(argv[0], argv, envp);
		delete[] envp;
		exit(EXIT_FAILURE); // check this
	}
	// * Parent Process
	close(pipe_fd_aux[0]);
	write(pipe_fd_aux[1], req->raw_body.data(), req->raw_body.size()); // send request to cgi
	close(pipe_fd_aux[1]);

	close(pipe_fd[1]); // Close write end
	close(pipe_fd[0]); // Close read end
	this->stream->cgi_pid = pid;
	return 1;
}

/*
 * Delete the specified file;
 * Throws an error when is a directory;
 * Throws an error when it can't delete the file;
*/
void Res::exec_delete(void)
{
	std::string path = stream->req->file_path;

	path = path[0] == '/' ? path.substr(1) : path;
	if (stream->req->path_type == _DIRECTORY)
		throw HttpError("403", "We can't delete a directory.");
	else if (std::remove(path.c_str()) != 0)
		throw HttpError("404", "Couldn't delete the file");
	else
	{
		this->content = "We've deleted the file succesfully!\n";
		this->add_ext = ".txt";
		this->status_code = "200";
	}
}

/*
 * Builds the file as specified in the directory listing;
 * Read the speficied file;
 * Throws a Not Found when there no type of file;
*/
void Res::exec_get(void)
{
	if (stream->req->path_type == _FILE)
	{
		this->content = FileManager::read_file(stream->req->file_path);
		this->status_code = "200";
	}
	if (stream->req->path_type == _DIRECTORY)
	{
		this->content = FileManager::directory_listing(stream->req->file_path);
		this->add_ext = ".html";
		this->status_code = "200";
	}
	if (stream->req->path_type == _NONE)
		throw HttpError("404", "Not Found");
}

/*
 * Return the boundary from the content-type;
 * Throws errors if boundary is not well formatted;
*/
std::string get_boundary(const std::string &content_type)
{
	std::string boundary;
	size_t start_pos = content_type.find("=");

	if (start_pos == std::string::npos)
		throw HttpError("400", "Bad Request");
	boundary = content_type.substr(start_pos + 1);
	if (boundary[0] == '"' && boundary[boundary.length() - 1] == '"')
	{
		boundary.erase(0, 1);
		boundary.erase(boundary.length() - 1, 1);
	}
	return boundary;	
}


/*
 * * Only deals with content-type multipart/form-data
 * If the request body are files, saves them;
 * If the request body are regular form data, does nothing;
 */
void Res::exec_post(void)
{
	const std::string &content_type = stream->req->header["Content-Type"];
	Req *req = stream->req;
	std::string boundary;

	if (content_type.find("multipart/form-data;") != 1)
		throw HttpError("406", "We can't execute this type of request");
	boundary = get_boundary(content_type);
	this->status_code = FileManager::create_files(req->raw_body, boundary, req->file_path);
	if (this->status_code == "201")
		this->content = "What should be the content when we upload a file?";
}
