#include <FileManager.hpp>

FileManager::FileManager(/* args */) {}

FileManager::~FileManager() {}

std::string FileManager::get_extension(const std::string filename)
{
	size_t dotPos = filename.find_last_of('.');

	if (dotPos == std::string::npos || dotPos == filename.length() - 1)
		return (".txt");
	else
		return filename.substr(dotPos);
}

std::string FileManager::get_filename(const std::string filename)
{
	size_t dotPos = filename.find_last_of('/');

	if (dotPos == std::string::npos || dotPos == filename.length() - 1)
		return ("/");
	else
		return filename.substr(dotPos + 1);
}

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
	* Creates an html file with the list of the files and directories;
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
