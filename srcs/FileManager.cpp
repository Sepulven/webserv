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

static std::string read(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	std::stringstream buff;

	buff << file.rdbuf();
	return (buff.str());
}
