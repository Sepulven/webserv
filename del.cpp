#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static std::vector<std::string> split(std::string& base, std::string delimiter)
{
    std::istringstream iss(base);
    std::string token;
    std::vector<std::string> tokens;

    size_t pos = base.find(delimiter);
    while (pos != std::string::npos)
    {
        token = base.substr(0, pos);
        tokens.push_back(token);
        base.erase(0, pos + delimiter.length());

        pos = base.find(delimiter); // Find next occurrence of the delimiter
    }
    tokens.push_back(base); // Add the remaining part after the last delimiter
    return tokens; // Return the tokens vector by value (a copy is made)
}

int main() {
    std::string input = "abc:def:ghi";
    std::string delimiter = ":";

    std::vector<std::string> result = split(input, delimiter);

    // Accessing the return values outside of the split function scope
    for (const std::string& token : result) {
        std::cout << token << std::endl;
    }

    return 0;
}
