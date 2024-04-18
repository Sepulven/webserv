#include <Req.hpp>

std::pair<std::string, std::string> Req::split(std::string str, char c)
{
    std::size_t found = str.find(c);
    std::string s1 = str.substr(0, found);
    std::string s2 = str.substr(found + 1);
    
    return std::make_pair(s1, s2);
}

std::string Req::readFile(void)
{
    // missing: looking for file in root (present in config file) ?

    if (opendir(location.c_str()) != NULL && this->index != "")
        this->file_to_open = this->index;
    else
        this->file_to_open = this->location;

    std::ifstream file(file_to_open.c_str());
    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

std::string Req::intToString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}
