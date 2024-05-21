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
	content_type[".ico"] = "image/x-icon";
	content_type[".png"] = "image/png";
	content_type[".jpeg"] = "image/jpeg";
	content_type[".jpg"] = "image/jpeg";


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

	std::cout << "check pages 0\n";
	if (status_code[0] != '2') // * There is an error;
		this->content = FileManager::build_error_pages(error_pages, status_code, error_msg);
	std::cout << "check pages\n";
	ss << "HTTP/1.1 " << status_code << " " << this->status[status_code] << "\r\n";
	ss << "Content-Type: " << content_type[req->file_ext] << "\r\n";
	ss << "Content-Length: " << content.length() << "\r\n\r\n";
	ss << content;

	this->data = ss.str();
	return (write(stream->fd, this->data.c_str(), this->data.length()));
}

int	Res::check_method(void)
{
	std::cout << "path: " << stream->req->file_path << std::endl;
	std::cout << "method: " << stream->req->method << std::endl;
	std::cout << "is route: " << stream->req->is_route << std::endl;
	int i = 0;

	// check is route logic
	// is_route is set when the path is expanded
	// problem: should we check the root of every route or just the route? uploads example
	if (stream->req->is_route != "")
	{
		while (stream->req->is_route != this->stream->server->routes[i].name)
			i++;
		std::cout << "route: " << stream->req->is_route << std::endl;
		std::cout << "name: " << stream->server->routes[i].name << std::endl;
		for (size_t f = 0; f < stream->server->routes[i].http_methods.size(); f++)
			if (stream->req->method == stream->server->routes[i].http_methods[f])
				return 1;
	}
	else // when path is not route, check http methods of root of the server
	{
		for (size_t g = 0; g < stream->server->routes.back().http_methods.size(); g++)
			if (stream->req->method == stream->server->routes.back().http_methods[g])
				return 1;
	}
	return -1;
}

/*
 * Must check for the permissions before executing;
 * Execute the action of each individual method;
 * In case of error during the execution, changes the state of the response;
 */

int Res::send(void)
{
	Req *req = stream->req;

	std::vector<std::string> &cgi_path = req->cgi_path;
	std::vector<std::string>::iterator it = std::find(cgi_path.begin(), cgi_path.end(), req->file_path);

	if (!this->status_code.empty() && !this->error_msg.empty())
		return (build_http_response());
	std::cout << "File ext: " << req->file_ext << std::endl;
	// curr_expansion == this->stream->server->routes.back().root;
	// else {
	// 	std::cout <<  "ERRO FDD\n";
	// 	this->content = FileManager::read_file("error/403.html"); // change for error page variable
	// 	this->add_ext = ".html";
	// 	this->status_code = "403";
	// 	return (build_http_response());
	// }
	if (this->check_method() == -1)
	{
		this->content = FileManager::read_file("error/403.html"); // change for error page variable
		this->add_ext = ".html";
		this->status_code = "403";
		return (build_http_response());
	}
	std::cout << "check 0\n";
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
		std::cout << "check msg "<< this->error_msg << std::endl;
		this->status_code = e.get_status();
		std::cout << "check status " << this->status_code << std::endl;
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
 * Check if dir listing in on for two possible claes: route or root
*/
int	Res::check_dir_listing(size_t i)
{
	if (i < this->stream->server->routes.size() && stream->server->routes[i].dir_listing == 1)
		return 1;
	else if (i == this->stream->server->routes.size() && stream->server->routes.back().dir_listing == 1)
		return 1;
	return -1;
}

std::string	Res::check_index(size_t i)
{
	std::string	name_aux;
	std::ifstream file;

	for (size_t g = 0; g < this->stream->server->routes[i].index.size(); g++) {
		name_aux = this->stream->req->file_path  + this->stream->server->routes[i].index[g];
		file.open(&name_aux.c_str()[name_aux.c_str()[0] == '/']);
		if (file.is_open())
		{
			file.close();
			return name_aux;
		}
	}
	return "";
}

/*
 * Builds the file as specified in the directory listing;
 * Read the speficied file;
 * Throws a Not Found when there no type of file;
*/
void Res::exec_get(void)
{
	std::cout << "\n\nFile PATH: " << stream->req->file_path << std::endl;
	size_t i = 0;
	while (i < this->stream->server->routes.size() && stream->req->is_route != this->stream->server->routes[i].name)
		i++;
	
	if (stream->req->path_type == _FILE) {
		this->content = FileManager::read_file(stream->req->file_path);
		this->status_code = "200";
	}
	if (stream->req->path_type == _DIRECTORY) {
		if (stream->req->file_path[stream->req->file_path.size() - 1] != '/') {
			stream->req->file_path += "/";
		}
		if (this->check_dir_listing(i) == 1)
		{
			this->content = FileManager::directory_listing(stream->req->file_path);
			this->add_ext = ".html";
			this->status_code = "200";
		}
		else if (this->check_index(i) != "")
		{
			std::string name = this->check_index(i);
			this->content = FileManager::read_file(name);
			this->status_code = "200";
		}
		else
			throw HttpError("403", "Forbidden");
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
	std::cout << "post files in: " << req->file_path << std::endl;

	DIR* dir = opendir(req->file_path.c_str());
	if (!dir) {
		std::cout << "dir does not exist\n " ;
        throw HttpError("500", "Internal Server Error");
    }

	this->status_code = FileManager::create_files(req->raw_body, boundary, req->file_path);
	if (this->status_code == "201")
		this->content = "What should be the content when we upload a file?";
}
