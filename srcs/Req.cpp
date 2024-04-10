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
    
    // method and url
    std::string aux = this->line.substr(i, f);
    this->method = (split(aux, ' ')).first;
    std::string aux1 = (split(aux, ' ')).second;
    this->url = (split(aux1, ' ')).first;

    i = f;
    std::string rest = this->line.substr(f + 1);
    f = rest.find("\n");
    aux = this->line.substr(i + 1, f);
    while (f < this->line.size())
    {
        this->elements.insert(split(aux, ':'));
        i = f;
        rest = rest.substr(f + 1);
        f = rest.find("\n");
        aux = rest.substr(0, f);
    }
}

// missing error cases (wrong config files)
void    Req::get_request(void)
{
    this->map_elements();

    std::map<std::string, std::string>::iterator it = this->elements.begin();
    while (it != this->elements.end())
    {
        std::cout << "check\n";
        std::cout << "Map " << it->first << " | " << it->second;
        it++;
    }
}
