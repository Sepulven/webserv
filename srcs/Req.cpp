#include <Req.hpp>

Req::Req(int connection, std::string line_w)
{
    this->line = line_w;
	std::cout << "<<<<<<<<<<<<<<<\n" <<  this->line << std::endl;

    if (!this->line.size())
        this->send_response("400");

    this->con = connection;

    // defining elements from config file
    this->autoindex = true;
    this->index = "index.html";
    this->redirect = "";
    this->get_allowed = true;
    this->post_allowed = true;
    this->delete_allowed = true;

    // define default error pages if not specified in file
    this->error_pages["400"] = "./error/400.html";
    this->error_pages["404"] = "./error/404.html";
}

void    Req::map_elements()
{
    long unsigned int i = 0;
    long unsigned int f = this->line.find("\n");

    std::string m = this->line.substr(0, f);
    this->method = (split(m, ' ')).first;
    
    i = f;
    std::string rest = this->line.substr(f + 1);
    f = rest.find("\n");
    std::string aux = this->line.substr(i + 1, f - 1);
    while (f < this->line.size())
    {
        this->elements.insert(split(aux, ':'));
        i = f;
        rest = rest.substr(f + 1);
        f = rest.find("\n");
        aux = rest.substr(0, f - 1);
    }
}

void    Req::get_info()
{
    long unsigned int f = this->line.find("\n");
    std::string aux = this->line.substr(0, f);
    std::string aux1 = (split(aux, ' ')).second;

    if (this->method == "GET" || this->method == "DELETE")
        this->location = (split(aux1, ' ')).first;

    this->connection = (this->elements)["Connection"];

    if (this->method == "POST")
    {
        this->cont_type = (this->elements)["Content-Type"];
        this->cont_len = (this->elements)["Content-Length"];

        int pos = this->line.find("Content-Disposition");
        this->body = this->line.substr(pos);

        std::string req = this->line;
        int i = req.find("filename=") + 10;
        req = req.substr(i);
        int f = req.find("\"");
        this->filename = req.substr(0, f);
    }
}

void    Req::response_directory()
{
    std::cout << "listing" << std::endl;
    std::string listing;
    listing += "<!DOCTYPE html><html lang=\"en\"><html><head><title>Directory Listing</title></head><body><h1>Directory Listing</h1><ul>";
    
    DIR* dir = opendir(location.c_str());
    if (dir != NULL) {
        struct dirent* entry;

        std::string aux;
        if (location[location.size() - 1] != '/')
            aux = location.substr(2) + '/';
        else
            aux = location;

        while ((entry = readdir(dir)) != NULL)
        {
            std::string link = "<li><a href=\"/" + aux + std::string(entry->d_name) + "\" target=\"_blank\">" + std::string(entry->d_name) + "</a></li>";
            std::cout << "new link: " << link << std::endl;
            listing += "<li><a href=\"/" + aux + std::string(entry->d_name) + "\" target=\"_blank\">" + std::string(entry->d_name) + "</a></li>";
        }
        closedir(dir);
    }
    else
        listing += "<li>Error: Unable to open directory</li>";

    listing += "</ul></body></html>";

    std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + intToString(listing.length()) + "\n\n" + listing;
    std::cout << ">>>>>>>>>>>>>>>\n" << response << std::endl;
    write(this->con, response.c_str(), response.size());
}

std::string Req::get_extension(std::string str)
{
    std::string extension;
    size_t i = str.find_last_of(".");
    if (i == str.npos)
    {
        std::cout << "file not supported\n";
        this->send_response("400");
    }
    else
        extension = str.substr(i);
    return extension;
}

std::string Req::get_response_body(std::string code)
{
    std::string file_name;
    std::string content;

    if (code != "200")
    {
        file_name = this->error_pages[code];
        std::ifstream file(file_name.c_str());
        std::stringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str();
    }
    else
    {
        if (method == "GET")
            content = this->readFile();
        else if (method == "POST")
            content = "File uploaded successfully!";
        else if (method == "DELETE")
            content = "File deleted successfully!";
    }
    return content;
}

void    Req::send_response(std::string code)
{
    std::string file_name;
    std::string status;
    std::string extension = "";

    std::string content = get_response_body(code);

    if (code == "200")
    {
        status = "OK";
        if (method == "GET")
            extension = this->get_extension(this->file_to_open);
        else if (method == "POST")
            extension = this->get_extension(this->filename);
        else if (method == "DELETE")
            extension = this->get_extension(this->location);
    }
    else
    {
        if (code == "400")
            status = "BAD REQUEST";
        else if (code == "403")
            status = "FORBIDDEN";
        else if (code == "404")
            status = "NOT FOUND";
        extension = ".html";
    }

    static std::map<std::string, std::string> content_type;
    content_type[".txt"] = "text/plain";
    content_type[".cpp"] = "text/plain";
    content_type[".hpp"] = "text/plain";
    content_type[".html"] = "text/html";
    content_type[".pdf"] = "application/pdf";

    std::string response = "HTTP/1.1 " + code + " " + status + "\r\nContent-Type: " + content_type[extension] + "\r\nContent-Length: " + intToString(content.length()) + "\r\n\r\n" + content;
    std::cout << ">>>>>>>>>>>>>>>\n" << response << std::endl;

    write(this->con, response.c_str(), response.size());

    if (code != "200")
        throw std::invalid_argument("");
}

void    Req::send_file()
{
    if (!this->get_allowed)
        this->send_response("400");
    
    if (location[0] == '/')
        location = "." + location;

    if (open(location.c_str(), O_RDONLY) == -1)
        this->send_response("404");

    DIR* dir = opendir(location.c_str());
    if (dir != NULL)  // directory
    {
        if (this->index != "")
        {
            if (open(location.c_str(), O_RDONLY) == -1)
                this->send_response("404");
            this->send_response("200");
        }
        else if (this->autoindex)
            this->response_directory();
        else
            this->send_response("403");
    }
    else  // file
    {
        std::cout << "loc: " << location << std::endl;
        if (open(location.c_str(), O_RDONLY) == -1)
            this->send_response("404");
        this->send_response("200");
    }
}

void    Req::create_file()
{
    // if (location[location.size() - 1] == '/') // directory
    //     this->send_response("403");

    if (!this->post_allowed)
        this->send_response("400");
    
    std::ofstream file; // check if files exists
    file.open(this->filename.c_str(), std::ios::in | std::ios::binary);
    if (file.is_open())
        this->send_response("400");

    std::ofstream c_file(this->filename.c_str()); // create file
    if (!c_file)
        this->send_response("400");

    c_file << this->body; // populate file

    this->send_response("200");
}

void    Req::delete_file()
{
    // if (location[location.size() - 1] == '/') // directory
    //     this->send_response("403");

    if (!this->delete_allowed)
        this->send_response("400");

    if (this->location[0] == '/')
        location = location.substr(1);

    if (std::remove(location.c_str()) != 0)
        this->send_response("404");
    
    // std::string content = "File deleted successfully!";

    // std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + intToString(content.length()) + "\r\n\r\n" + content + "\r\n\r\n";
    // std::cout << ">>>>>>>>>>>>>>>\n" << response << std::endl;

    // write(this->con, response.c_str(), response.size()); // send file

    this->send_response("200");
}

void    Req::process_request(void)
{
    // response header (check date, server, etc)

    // post request status
    // moved temporarily and permanentely
    // method not allowed - new error or 400 ?
    // not supported types files

    // check if opening any dir should open the index, when the index is defined

    this->map_elements();

    try
	{
        if (this->method != "GET" && this->method != "POST" && this->method != "DELETE")
            this->send_response("400");

        this->get_info();

        std::cout << this->method << std::endl;

        if (this->redirect != "")
        {
            this->location = this->redirect;
            std::cout << "Moved temporarily" << std::endl;
        }
        
        if (this->method == "GET")
            this->send_file();
        else if (this->method == "POST")
        {
            std::cout << "check post" << std::endl;
            this->create_file();
        }
        else if (this->method == "DELETE")
            this->delete_file();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
