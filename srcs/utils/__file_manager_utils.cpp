#include <__file_manager_utils.hpp>
	
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
*/
std::string FileManager::directory_listing(const std::string path)
{
	std::stringstream ss;
	DIR* dir = opendir(path.c_str());
	struct dirent* entry;

	ss << "<!DOCTYPE html><html lang='en'>"
		<< "<html><head><title>Directory Listing</title></head>" 
		<< "<body><h1>Directory Listing</h1><ul>";
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
 * Returns the file content;
 * Takes care of the string manipulation;
*/
std::vector<uint8_t> get_file(const std::vector<uint8_t>& content)
{
	std::vector<uint8_t> file;

	size_t pos = RawData::find(content, "\r\n\r\n") + 4;
	size_t length = content.size() - pos - 2;

	file = RawData::substr(content, pos, length);
	return (file);
}

/*
 * Returns the status code of the operation;
*/
std::string FileManager::create_files(const std::vector<uint8_t>& body, const std::string & boundary, const std::string dir)
{
	std::vector<std::vector<uint8_t> > files = RawData::split(body, "--" +	 boundary);
	std::ofstream out_file;
	std::string filename;
	std::vector<uint8_t> file;


	std::cout << "files size " << files.size() << std::endl;

	for (size_t i = 1; i < files.size() - 1; i++)
	{
		file = get_file(files[i]);
		filename = dir + "/" + get_random_filename() + "_" + (char)(i+48);

		out_file.open(filename.c_str(), std::ios::binary);
		if (!out_file.is_open())
			return ("500");
		out_file.write((const char*)&file[0], file.size());
		if (out_file.fail())
			return ("500");
		out_file.close();
	}
	return ("201");
}

/*
 * Returns the status code of the operation;
*/
std::string FileManager::create_file(const std::vector<uint8_t>& body, const std::string dir)
{
	std::ofstream out_file;
	std::string filename = dir + "/" + get_random_filename() + "_chunked";

	out_file.open(filename.c_str(), std::ios::binary);

	if (!out_file.is_open())
		return ("500");

	out_file.write((const char*)&body[0], body.size());

	if (out_file.fail())
		return ("500");

	out_file.close();
	return ("201");
}

std::string FileManager::get_random_filename(void) 
{
	struct timeval		t;
	std::stringstream filename;

	gettimeofday(&t, NULL);
	filename << (t.tv_sec * 1000) + (t.tv_usec / 1000);
	return (filename.str());
}

