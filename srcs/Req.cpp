#include <Req.hpp>

Req::Req(int connection)
{
    char buffer[1024];
    int bytes_received = read(connection, buffer, sizeof(buffer));
    this->line = buffer;
	std::cout << "Request: \n" <<  this->line << "." << std::endl;
    while (!this->line.find("\r\n\r\n"))
    {
        buffer[bytes_received] = '\0';
        this->line =+ buffer;
        bytes_received = read(connection, buffer, sizeof(buffer));
    }
    if (!this->line.size())
        throw std::invalid_argument("Invalid request - empty");

    this->con = connection;

    // defining elements from config file
    this->autoindex = true;
    this->index = "";
    this->redirect = "";
    this->get_allowed = true;
    this->post_allowed = true;
    this->delete_allowed = true;
}

std::pair<std::string,std::string> split(std::string str, char c)
{
    std::size_t found = str.find(c);
    std::string s1 = str.substr(0, found);
    std::string s2 = str.substr(found + 1);
    
    return std::make_pair(s1, s2);
}

std::string Req::readFile(int dir)
{
    // missing: looking for file in root (present in config file)

    if (dir == 1)
        this->file_to_open = this->index;
    else
        this->file_to_open = this->location;

    std::ifstream file(file_to_open.c_str());
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::cout << buffer.str() << std::endl;

    return buffer.str();
}

std::string intToString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
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

    if (this->method == "GET")
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

#include <dirent.h>

void    Req::response_directory()
{
    std::cout << "listing" << std::endl;
    std::string listing;

    listing += "<!DOCTYPE html><html lang=\"en\"><html><head><title>Directory Listing</title></head><body><h1>Directory Listing</h1><ul>";
    std::cout << location << std::endl;
    DIR* dir = opendir(location.c_str());
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            // listing += "<li><a href=\"" + std::string(entry->d_name) + "\">" + std::string(entry->d_name) + "</a></li>"; - links
            listing += "<li>" + std::string(entry->d_name) + "</li>";
        }
        closedir(dir);
    } else {
        listing += "<li>Error: Unable to open directory</li>";
    }

    listing += "</ul></body></html>";

    std::cout << "Listing:\n" << listing << std::endl;

    std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + intToString(listing.length()) + "\n\n" + listing;

    write(this->con, response.c_str(), response.size());
}

void    Req::response_file(int dir)
{
    if (open(location.c_str(), O_RDONLY) == -1)
        throw std::invalid_argument("Invalid request - file not found");

    std::string content = this->readFile(dir);
    std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + intToString(content.length()) + "\n\n" + content;

    write(this->con, response.c_str(), response.size());
}

void    Req::send_file()
{
    if (!this->get_allowed)
        throw std::invalid_argument("Invalid request - method not allowed");

    if (location[0] == '/')
        location = "." + location;
    if (location[location.size() - 1] == '/') // directory
    {
        if (this->index != "")
            this->response_file(1);
        else if (this->autoindex)
            this->response_directory();
        else
            throw std::invalid_argument("Invalid request - file not found");
    }
    else
        this->response_file(0);
}

void    Req::create_file()
{
    if (!this->post_allowed)
        throw std::invalid_argument("Invalid request - method not allowed");
    
    std::ofstream file; // check if files exists
    file.open(this->filename.c_str(), std::ios::in | std::ios::binary);
    if (file.is_open())
        throw std::invalid_argument("Invalid request - file already exists");

    std::ofstream c_file(this->filename.c_str()); // create file
    if (!c_file)
        throw std::invalid_argument("Invalid request - error creating file");

    c_file << this->body; // populate file
    
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nFile uploaded successfully!";
    write(this->con, response.c_str(), response.size()); // send file
}

void    Req::delete_file()
{
    if (!this->delete_allowed)
        throw std::invalid_argument("Invalid request - method not allowed");
}

void    Req::process_request(void)
{
    this->map_elements();

    // missing
    // status codes + server response (cant exit)
    // error pages in browser
    // check links directory listing
    // check request - permissions
    // delete file

    if (this->method != "GET" && this->method != "POST" && this->method != "DELETE")
        throw std::invalid_argument("Invalid request - invalid method");

    this->get_info();

    std::cout << this->connection << std::endl;
    std::cout << this->location << std::endl;
    std::cout << this->method << std::endl;

    if (this->redirect != "")
    {
        this->location = this->redirect;
        std::cout << "Moved temporarily" << std::endl;
    }

    std::cout << this->location << std::endl;

    if (this->method == "GET")
        this->send_file();
    else if (this->method == "POST")
        this->create_file();
    else if (this->method == "DELETE")
        this->delete_file();
}
