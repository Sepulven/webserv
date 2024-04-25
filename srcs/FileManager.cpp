#include <FileManager.hpp>

//* Utils
static std::vector<std::string> split(const std::string& , const std::string& );


FileManager::FileManager(/* args */) {}

FileManager::~FileManager() {}

/*
	* Reads file given a path;
	* If path starts with '/', jumps it;

	TODO: Get the error page from the route;
	TODO: Set the correct status code;
*/
std::string FileManager::read_file(const std::string path)
{
	std::ifstream file;
	std::ifstream not_found("error/404.html");
	std::stringstream buff;

	file.open(&path.c_str()[path.c_str()[0] == '/']);

	if (!file.is_open())
	{
		if (!not_found.is_open())
			std::cout << "WE COULND'T OPEN THE ERROR FILE" << std::endl;
		buff << not_found.rdbuf();
	}
	if (file.is_open())
		buff << file.rdbuf();
	file.close();
	not_found.close();
	return (buff.str());
}

/*
	* Creates an html file with the list of the files and directories;
	TODO: Add better styling to the html page;
*/
std::string FileManager::directory_listing(const std::string path)
{
	std::stringstream ss;
	DIR* dir = opendir(path.c_str());
	struct dirent* entry;

	ss <<"<!DOCTYPE html><html lang=\"en\"><html><head><title>Directory Listing</title></head><body><h1>Directory Listing</h1><ul>";

	entry = readdir(dir);
	while (entry)
	{
		ss  << "<li>"
			<< "<a href='./" << entry->d_name << "'>" << entry->d_name << "</a>"
			<< "</li>" << std::endl;
		entry = readdir(dir);
	}

	ss << "</ul></body></html>" << std::endl;

	closedir(dir);
	return (ss.str());
}

/*
	TODO: Add better error handling;
*/
std::string FileManager::create_file(const std::string filename, const std::string content)
{
	std::ofstream file;

	file.open(filename.c_str(), std::ios::in | std::ios::binary);
	std::ofstream content_file(filename.c_str());

	content_file << content;
	file.close();
	return ("200");
}

/*
	* aaa
*/
std::string FileManager::create_files(const std::string &body, const std::string & boundary, const std::string dir)
{
	std::vector<std::string> files = split(body, boundary);
	std::string filename;
	std::string file;
	size_t n_files = files.size();

	(void)dir;

	for (size_t i = 0; i < n_files; i++)
	{
			std::cout 
					<< "[" << i << "]" << std::endl
					<< files[i]
					<< "**************************************" << std::endl;
	}
	return ("200");
}

// * Utils
static std::vector<std::string> split(const std::string& base, const std::string& delimiter)
{
	std::istringstream iss(base);
	std::string token;
	std::vector<std::string> tokens;
	size_t startPos = 0;

	size_t pos = base.find(delimiter, startPos);
	while (pos != std::string::npos)
	{
		token = base.substr(startPos, pos - startPos); 
		tokens.push_back(token);

		startPos = pos + delimiter.length();
		pos = base.find(delimiter, startPos);
	}
	tokens.push_back(base.substr(startPos));
	return tokens;
}
