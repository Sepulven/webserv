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
	* Log the response on stdout;
*/
void Res::log(void) const {
	std::cout << "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
	// size_t pos = this->data.find("\r\n\r\n");
	// std::string str_to_print = this->data.substr(0, pos + 2);
	std::cout << "<Log Response><>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
	std::cout << this->data;
}

/*
	* Must check for the permissions before executing;
	* Must handle in case of the URL is a  directory;
*/
int Res::send(void)
{
	Req *req = stream->req;
	std::stringstream ss;	

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

	std::cout << "here we are " << std::endl;
	if (req->method == "GET")
		exec_get();
	else if (req->method == "POST")
		exec_post();
	else if (req->method == "DELETE")
		exec_delete();

	ss << "HTTP/1.1 " << code << " " << this->status[code] << "\r\n";

	if (this->add_ext != "")
		ss << "Content-Type: " << content_type[add_ext] <<  "\r\n";
	else
		ss << "Content-Type: " << content_type[stream->req->file_ext] <<  "\r\n";

	ss << "Content-Length: " << content.length() << "\r\n\r\n";

	ss << content;

	this->data = ss.str();
	std::cout << this->data << std::endl;
	return (write(stream->fd, this->data.c_str(), this->data.length()));
}

int    Res::exec_CGI(void)
{
	Req *req = stream->req;

	char *argv0;
	if (req->file_ext == ".py")
    	argv0 = const_cast<char *>("/usr/bin/python3");
	else if (req->file_ext == ".php")
    	argv0 = const_cast<char *>("/usr/bin/php");
	else
		argv0 = const_cast<char *>("");

    char *argv1 = const_cast<char *>(req->file_path.c_str());
    char *const argv[] = {argv0, argv1, NULL};

    int pipe_fd[2];
    pipe(pipe_fd);

    pid_t pid = fork();

	std::string raw_body(req->raw_body.begin(), req->raw_body.end());
	std::string data(req->data.begin(), req->data.end());
    if (pid == 0) {
        std::vector<std::string> request;
        request.push_back("request=" + data);
        request.push_back("path=" + req->file_path);
        request.push_back("method=" + req->method);
        request.push_back("body=" + raw_body);
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
		std::cout << "\n>>>>>>>>>>>>>>>\n" << content;
		return(write(stream->fd, content.c_str(), content.length()));
    }
	return 1;
}

void	Res::exec_delete(void)
{
	// check permissions (deleting objects in allowed directory)
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

/*
 * * Only deals with content-type multipart/form-data
 * If the request body are files, saves them;
 * If the request body are regular form data, does nothing;
*/
void	Res::exec_post(void)
{
	Req *req = stream->req;
	const std::string & content_type = req->header["Content-Type"];
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
		this->code = FileManager::create_files(req->raw_body, boundary, "server_uploaded_files");
		if (this->code == "201")
			this->content = "What should be the content when we upload a file?";
		else
			this->content = "Error while dealing with your post request!";
	} 
	else
	{
		this->code = "405";
		this->content = "We can't execute this type of request";
	}
}
