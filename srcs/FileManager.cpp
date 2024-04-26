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
	TODO: Filename must be previous filename get_random_filename + extension

	! Edgy cases with pdf, images, binaries;
	! Edgy case, can't create file;
*/
std::string FileManager::create_files(const std::string &body, const std::string & boundary, const std::string dir)
{
	std::vector<std::string> files = split(body, "--" +	 boundary);
	std::string filename;
	std::ofstream out_file;
	size_t n_files = files.size();

	std::cout << body << std::endl;
	for (size_t i = 1; i < n_files - 1; i++)
	{
		filename = dir + "/" + get_random_filename();
		// std::cout << files[i] << std::endl;
		out_file.open(filename.c_str(), std::ios::binary | std::ios::trunc);
		out_file << files[i].substr(files[i].find("\r\n\r\n"));
		out_file.close();
	}
	return ("200");
}

std::string get_random_filename(void) 
{
	struct timeval		t;
	std::stringstream filename;

	gettimeofday(&t, NULL);
	filename << (t.tv_sec * 1000) + (t.tv_usec / 1000);
	return (filename.str());
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
