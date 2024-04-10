#include <Req.hpp>

Req::Req(int connection)
{
    char buffer[1024];
    int bytes_received = read(connection, buffer, sizeof(buffer));
    this->line = buffer;
    while (!this->line.find("\r\n\r\n"))
    {
        buffer[bytes_received] = '\0';
        this->line =+ buffer;
        bytes_received = read(connection, buffer, sizeof(buffer));
    }
	std::cout << "Request: \n" <<  this->line << "." << std::endl;
}

std::pair<std::string,std::string> split(std::string str, char c)
{
    std::size_t found = str.find(c);
    std::string s1 = str.substr(0, found);
    std::string s2 = str.substr(found + 1);
    
    return std::make_pair(s1, s2);
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
        this->url = (split(aux1, ' ')).first;
    this->connection = (this->elements)["Connection"];

    if (this->method == "POST")
    {
        this->cont_type = (this->elements)["Content-Type"];
        this->cont_len = (this->elements)["Content-Length"];
    }
}

void    Req::get_request(void)
{
    this->map_elements();

    // std::map<std::string, std::string>::iterator it = this->elements.begin();
    // while (it != this->elements.end())
    // {
    //     std::cout << it->first << " | " << it->second;
    //     it++;
    // }

    // check request - permissions and existance ?
    std::cout << this->connection << std::endl;
    std::cout << this->url << std::endl;
    std::cout << this->method << std::endl;

    if (this->method != "GET" && this->method != "POST" && this->method != "DELETE")
        throw std::invalid_argument("Invalid request");

    
    this->get_info();


}
