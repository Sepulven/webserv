#include <Res.hpp>

/*
 * We must have this map globally;
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

	this->c_type_response = "";
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
	std::stringstream ss;
	std::string extension = "";

	if (status_code[0] != '2') // * There is an error;
	{
		this->content = FileManager::build_error_pages(error_pages, status_code, error_msg, extension);
		c_type_response = content_type[FileManager::set_file_ext(extension)];
	}
	if (c_type_response == "")
		c_type_response = "text/plain";
	ss << "HTTP/1.1 " << status_code << " " << this->status[status_code] << "\r\n";
	ss << "Content-Type: " << c_type_response << "\r\n";
	ss << "Content-Length: " << content.length() << "\r\n\r\n";
	ss << content;

	this->data = ss.str();
	return (write(stream->fd, this->data.c_str(), this->data.length()));
}

/*
 * Check method for route;
 * Returns 1 in success;
 * Returns -1 in failure;
 */
int	Res::check_method(void)
{
	int i = this->stream->req->route_id;
	for (size_t f = 0; f < stream->server->routes[i].http_methods.size(); f++)
		if (stream->req->method == stream->server->routes[i].http_methods[f])
			return 1;
	return -1;
}

int	Res::is_redirection(void)
{
	int i = this->stream->req->route_id;
	if (stream->server->routes[i].redirect != "")
	{
		std::stringstream ss;
		ss << "HTTP/1.1 302 Found\r\n";
		ss << "Location: " << stream->server->routes[i].redirect << "\r\n";
		// ss << "Content-Length: " << content.length() << "\r\n\r\n";
		ss << content;

		this->data = ss.str();
		write(stream->fd, this->data.c_str(), this->data.length());
		return 1;
	}
	return 0;
}

/*
 * Must check for the permissions before executing;
 * Execute the action of each individual method;
 * In case of error during the execution, changes the state of the response;
 */
int Res::send(void)
{
	Req *req = stream->req;

	if (!this->status_code.empty() && !this->error_msg.empty())
		return (build_http_response());

	stream->server->routes[0].redirect = "srcs/main.cpp";
	try
	{
		if (this->is_redirection() == 1)
			return 1;
		if (this->check_method() == -1)
			throw HttpError("403" , "Forbidden");
		if (stream->server->cgi_path.find(req->file_ext) != stream->server->cgi_path.end())
			return (this->exec_CGI());
		if (req->method == "GET") {
			if (exec_get() == 1)
				return 1;
		}
		else if (req->method == "POST") {
			if (exec_post() == 1)
				return 1;
		}
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
	pid_t pid;
	
	ServerContext * server = stream->server;
	char *argv0 = const_cast<char *>(server->cgi_path[req->file_ext].c_str());
	char *argv1 = const_cast<char *>(req->file_path.c_str());
	char *const argv[] = {argv0, argv1, NULL};

	pipe(pipe_fd);
	pid = fork();

	if (pid == 0)
	{
		std::vector<std::string> request;
		int dev_null;

		request.push_back("request=" + data);
		request.push_back("method=" + req->method);
		request.push_back("path=" + req->file_path);
		request.push_back("body=" + raw_body);

		char **envp = new char *[request.size() + 1];
		size_t i = 0;
		for (i = 0; i < request.size(); i++)
		{
			envp[i] = new char[request[i].size() + 1];
			std::strcpy((char *)envp[i], request[i].c_str());
		}
		envp[i] = NULL;
		dup2(stream->fd, STDOUT_FILENO); // Redirect stdout to the fd

		close(pipe_fd[1]); // Close write end
		dup2(pipe_fd[0], STDIN_FILENO); // Redirect stdin to the read end

		dev_null = open("/dev/null", O_WRONLY);
		dup2(dev_null, STDERR_FILENO); // redirecting stderr to /dev/null
		close(dev_null);

		execve(argv[0], argv, envp);
		delete[] envp;
		exit(EXIT_FAILURE); // check this
	}
	// * Parent Process
	close(pipe_fd[0]);
	write(pipe_fd[1], req->raw_body.data(), req->raw_body.size()); // send request to cgi
	close(pipe_fd[1]);

	stream->cgi_pid = pid;
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
	this->content = "We've deleted the file succesfully!\n";
	this->status_code = "200";
}

/*
 * Check if dir listing in on for two possible claes: route or root
*/
int	Res::check_dir_listing(void)
{
	size_t	i = this->stream->req->route_id;
	if (stream->server->routes[i].dir_listing == 1)
		return 1;
	return -1;
}

std::string	Res::check_index(void)
{
	std::string	name;
	std::ifstream file;

	int i = this->stream->req->route_id;

	for (size_t g = 0; g < this->stream->server->routes[i].index.size(); g++) {
		name = this->stream->req->file_path + this->stream->server->routes[i].index[g];
		file.open(&name.c_str()[name.c_str()[0] == '/']);
		if (file.is_open())
		{
			file.close();
			return name;
		}
	}
	return "";
}

// std::string	Res::set_file_ext(std::string name)
// {
// 	size_t pos;
// 	std::string	file_ext_res = "";

// 	pos = name.find_last_of('.');
// 	if (pos == std::string::npos && pos >= name.length())
// 		file_ext_res = ".txt";
// 	else
// 		file_ext_res = name.substr(pos);
// 	return file_ext_res;
// }

/*
 * Builds the file as specified in the directory listing;
 * Read the speficied file;
 * Throws a Not Found when there no type of file;
*/
int Res::exec_get(void)
{
	Req * req = stream->req;
	
	if (req->path_type == _FILE) {
		content = FileManager::read_file(req->file_path);
		c_type_response = content_type[FileManager::set_file_ext(req->file_path)];
		status_code = "200";
	}
	if (req->path_type == _DIRECTORY) {
		if (req->file_path[req->file_path.size() - 1] != '/')
			req->file_path += "/";
		if (check_dir_listing() == 1) {
			content = FileManager::directory_listing(req->file_path, req->route_path, stream->server->port);
			c_type_response = "text/html";
			status_code = "200";
		}
		else if (!check_index().empty()) {
			std::string name = check_index();
			if (!name.empty()) {
				if (stream->server->cgi_path.find(FileManager::set_file_ext(name)) != stream->server->cgi_path.end()) {
					req->file_path = "./" + name;
					req->file_ext = FileManager::set_file_ext(req->file_path);
					return (exec_CGI());
				}
				else {
					content = FileManager::read_file(name);
					c_type_response = content_type[FileManager::set_file_ext(name)];
				}
			status_code = "200";
			}
			else
				throw HttpError("404", "Not Found");
		}
		else
			throw HttpError("403", "Forbidden");
	}
	if (stream->req->path_type == _NONE)
		throw HttpError("404", "Not Found");
	return 0;
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
int Res::exec_post(void)
{
	const std::string &content_type = stream->req->header["Content-Type"];
	Req *req = stream->req;
	std::string boundary;

	if (req->path_type == _DIRECTORY) {
		if (req->file_path[req->file_path.size() - 1] != '/')
			req->file_path += "/";
		if (!check_index().empty()) {
			std::string name = check_index();
			if (stream->server->cgi_path.find(FileManager::set_file_ext(name)) != stream->server->cgi_path.end()) {
				req->file_path = "./" + name;
				req->file_ext = FileManager::set_file_ext(req->file_path);
				return (exec_CGI());
			}
		}
		else
			throw HttpError("404", "Not Found");
	}

	if (content_type.find("multipart/form-data;") != 1)
		throw HttpError("406", "We can't execute this type of request");
	
	boundary = get_boundary(content_type);
	DIR* dir = opendir(req->file_path.c_str());
	if (!dir)
		throw HttpError("500", "Internal Server Error");
	if (closedir(dir) != 0)
		throw HttpError("500", "Internal Server Error");

	this->status_code = FileManager::create_files(req->raw_body, boundary, req->file_path);
	if (this->status_code == "201")
		this->content = "What should be the content when we upload a file?";
	return 0;
}
