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
	std::cout << "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
	// size_t pos = this->data.find("\r\n\r\n");
	// std::string str_to_print = this->data.substr(0, pos + 2);
	std::string str_to_print = this->data;
	std::cout << str_to_print;
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
	std::vector<std::string>::iterator it = req->cgi_path.begin();
	while (it != req->cgi_path.end())
	{
		if (req->file_path == *it)
		{
			std::cout << "cgi " << req->file_path << std::endl;
			return (this->exec_CGI());
		}
		it++;
	}

	// if (req->cgi_path != "" && req->file_path == req->cgi_path)
	// 	return (this->exec_CGI());
	if (req->method == "GET")
		exec_get();
	else if (req->method == "POST")
		exec_post();
	else if (req->method == "DELETE")
		exec_delete();

	ss << "HTTP/1.1 " << code << " " << this->status[code] << "\r\n";
	std::cout << "ext: " << stream->req->file_ext << std::endl;
	if (this->add_ext != "")
		ss << "Content-Type: " << content_type[add_ext] <<  "\r\n";
	else
		ss << "Content-Type: " << content_type[stream->req->file_ext] <<  "\r\n";
	ss << "Content-Length: " << content.length() << "\r\n\r\n";
	ss << content;

	this->data = ss.str();
	this->log();
	return (write(stream->fd, this->data.c_str(), this->data.length()));
}

int    Res::exec_CGI(void)
{
	Req *req = stream->req;
    // if (location[0] == '/')
    //     location = "." + location;
    // if (location.find('?'))
    //     location = split(location, '?').first;

	char *argv0;
	std::cout << "file extension: " << req->file_ext << std::endl;
	if (req->file_ext == ".py")
    	argv0 = const_cast<char *>("/usr/bin/python3");
	else if (req->file_ext == ".php")
    	argv0 = const_cast<char *>("/usr/bin/php");
	else
		argv0 = const_cast<char *>("");
	std::cout << "argv0: " << argv0 << std::endl;
    char *argv1 = const_cast<char *>(req->file_path.c_str());
    char *const argv[] = {argv0, argv1, NULL};

    int pipe_fd[2];
    pipe(pipe_fd);

    pid_t pid = fork();

    if (pid == 0) {
        std::vector<std::string> request;
        request.push_back("request=" + req->data);
        request.push_back("path=" + req->file_path);
        request.push_back("method=" + req->method);
        request.push_back("body=" + req->body);
        request.push_back("content-type=" + content_type[req->file_ext]);
        // request.push_back("content-lenght=" + req->body.length());

        char **envp = new char*[request.size() + 1];
        size_t i = 0;
        for (i = 0; i < request.size(); i++) {
            envp[i] = new char[request[i].size() + 1];
            std::strcpy((char *)envp[i], request[i].c_str());
        }
        envp[i] = NULL;

        close(pipe_fd[0]); // Close read end
        dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to the write end

		int dev_null = open("/dev/null", O_WRONLY);
		dup2(dev_null, STDERR_FILENO); // redirecting stderr to /dev/null
		close(dev_null);

        execve(argv[0], argv, envp);
		delete []envp;
		exit(EXIT_FAILURE); // check this
    }
    else {

        close(pipe_fd[1]); // Close write end
        char buffer[4096];
        ssize_t bytes_read;
        std::string content = "";
        while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) // Read output from the pipe
            content.append(buffer, bytes_read);

        int status;
        close(pipe_fd[0]); // Close read end
        waitpid(pid, &status, 0); // Wait for the child process to finish
		if (content == "")
			content = "HTTP/1.1 500 Internal Server Error\nContent-Type:text/plain\nContent-Length: 20\r\n\r\nError running script\n";
		// std::cout << "\n>>>>>>>>>>>>>>>\n" << content;
		return(write(stream->fd, content.c_str(), content.length()));
    }
	return 1;
}

void	Res::exec_delete(void)
{
	std::string path = stream->req->file_path;

	if (path[0] == '/')
		path = path.substr(1);
	// missing extension / content type of this responses
	if (std::remove(path.c_str()) != 0)
	{
		this->content = FileManager::read_file("errors/404.html"); // change for error page variable
		this->add_ext = ".html";
		this->code = "404";
	}
	else
	{
		this->content = "We've deleted the file succesfully!";
		this->add_ext = ".txt";
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
		// missing extension / content type of this responses
		this->content = FileManager::directory_listing(stream->req->file_path);
		this->add_ext = ".html";
		this->code = "200";
	}
	if (stream->req->path_type == _NONE)
	{
		this->content = FileManager::read_file("errors/404.html");  // change for error page variable
		this->add_ext = ".html";
		this->code = "404";
	}
}

void	Res::exec_post(void)
{
	this->code = "200";
	this->content = FileManager::create_file(stream->req->filename, stream->req->body);
}

