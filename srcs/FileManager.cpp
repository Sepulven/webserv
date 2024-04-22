#include <FileManager.hpp>

FileManager::FileManager(/* args */)
{
}

FileManager::~FileManager()
{
}

std::string FileManager::get_extension(const std::string &filename)
{
	size_t dotPos = filename.find_last_of('.');

	if (dotPos == std::string::npos || dotPos == filename.length() - 1)
		return (".txt");
	else
		return filename.substr(dotPos);
}

std::string FileManager::get_filename(const std::string &filename)
{
	size_t dotPos = filename.find_last_of('/');

	if (dotPos == std::string::npos || dotPos == filename.length() - 1)
		return ("/");
	else
		return filename.substr(dotPos + 1);
}

std::string FileManager::read_file(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	std::ifstream not_found("error/404.html");
	std::stringstream buff;

	if (!file.is_open())
	{
		if (!not_found.is_open())
			std::cout << "WE COULND'T OPEN THE ERROR FILE" << std::endl;
		buff << not_found.rdbuf();
	}
	if (file.is_open())
		buff << file.rdbuf();
	return (buff.str());
}

std::string FileManager::create_file(const std::string& filename, const std::string& content)
{
	std::ofstream file;

	file.open(filename.c_str(), std::ios::in | std::ios::binary);
	if (file.is_open())
		return ("400");
	std::ofstream content_file(filename.c_str());
	if (!content_file)
		return ("400");
	
	content_file << content;
	return ("200");
}
