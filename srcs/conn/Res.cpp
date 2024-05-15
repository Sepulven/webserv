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
	std::stringstream ss;
	Req *req = stream->req;

	if (status_code[0] != '2') // * Sucess;
	{
		this->content = FileManager::build_error_pages("", this->status_code, this->error_msg);
		// * What if the page doesn't exist? We it does not exist, we build it;
	}
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

int	Res::check_method(void)
{
	Req *req = stream->req;

	// get dir of request (srcs/main.cpp -> srcs/)
	std::string dir;
	size_t found = req->file_path.find_last_of("/\\");
	if (found != std::string::npos)
		dir = req->file_path.substr(0, found);
	else
		dir = "";

	std::string path_aux;
	for (long unsigned int i = 0; i < this->stream->server->routes.size(); i++) {
		if (this->stream->server->routes[i].root != "") // root + location
			path_aux = this->stream->server->routes[i].root + "/" + this->stream->server->routes[i].path;
		else
			path_aux = this->stream->server->routes[i].path;
		// check if the path request matches this route
		if ((dir != "" && path_aux == dir) || (dir == "" && path_aux == req->file_path))
		{
			// check if method is allowed
			for (long unsigned int f = 0; f < this->stream->server->routes[i].http_methods.size(); f++) {
				if (req->method == this->stream->server->routes[i].http_methods[f])
					return (1);
			}
			return -1;
		}
	}
	return -1;
}

int Res::send(void)
{
	Req *req = stream->req;

	std::vector<std::string> &cgi_path = req->cgi_path;
	std::vector<std::string>::iterator it = std::find(cgi_path.begin(), cgi_path.end(), req->file_path);

	if (!this->status_code.empty() && !this->error_msg.empty())
		return (build_http_response());
	if (check_method() == -1)
	{
		this->content = FileManager::read_file("error/403.html"); // change for error page variable
		this->add_ext = ".html";
		this->status_code = "403";
		return (build_http_response());
	}
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

void Res::exec_delete(void)
{
	// check permissions (deleting objects in allowed directory)
	std::string path = stream->req->file_path;

	if (path[0] == '/')
		path = path.substr(1);
	// missing extension / content type of this responses
	if (stream->req->path_type == _DIRECTORY)
	{
		std::cout << "DIR\n";
		this->content = FileManager::read_file("error/403.html"); // change for error page variable
		this->add_ext = ".html";
		this->status_code = "403";
	}
	else if (std::remove(path.c_str()) != 0)
	{
		this->content = FileManager::read_file("error/404.html"); // change for error page variable
		this->add_ext = ".html";
		this->status_code = "404";
	}
	else
	{
		this->content = "We've deleted the file succesfully!\n";
		this->add_ext = ".txt";
		this->status_code = "200";
	}
}

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
	{
		this->content = FileManager::read_file("errors/404.html"); // change for error page variable
		this->add_ext = ".html";
		this->status_code = "404";
	}
}

/*
 * * Only deals with content-type multipart/form-data
 * If the request body are files, saves them;
 * If the request body are regular form data, does nothing;
 */
void Res::exec_post(void)
{
	Req *req = stream->req;
	const std::string &content_type = req->header["Content-Type"];
	std::string boundary;

	if (content_type.find("multipart/form-data;") == 1)
	{
		// * I am assuming that the boundary is well formated and it is there;
		boundary = content_type.substr(content_type.find("=") + 1);

		// * In case the boundary is inside quotes;
		if (boundary[0] == '"' && boundary[boundary.length() - 1] == '"')
		{
			boundary.erase(0, 1);
			boundary.erase(boundary.length() - 1, 1);
		}
		this->status_code = FileManager::create_files(req->raw_body, boundary, req->file_path);
		if (this->status_code == "201")
			this->content = "What should be the content when we upload a file?";
		else
			this->content = "Error while dealing with your post request!";
	}
	else
	{
		this->status_code = "406";
		this->content = "We can't execute this type of request";
	}
}
