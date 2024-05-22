#include <__file_manager_utils.hpp>
#include <string.h>


/*
 * Returns a html page given a content;
 * Styles it;
*/
std::string html_template(const std::string &content, const std::string &title)
{
	std::stringstream ss;

	ss << "<!DOCTYPE html><html lang='en'>"
		<< "<html><head><title>"<< title << "</title></head>" 
		<< "<body>";
	ss << content;
	ss << "</body></html>" << std::endl;
	return (ss.str());
}

/*
	* Returns the content of the error pages in case of success;
	* Builds an error page in case none exists;
*/
std::string FileManager::build_error_pages(const std::map<int, std::string> &error_pages, const std::string &code, const std::string &error_msg)
{
	std::ifstream file;
	std::stringstream content;
	std::string path = "";
	int error_code = atoi(code.c_str());

	try {
		path = error_pages.at(error_code);
	} catch (const std::out_of_range& e) {}

	file.open(&path.c_str()[path.c_str()[0] == '/']);
	// * If we can't open it, we build it;
	if (!file.is_open())
	{
		content << "<h1>"<< code << " " << "</h1>"
				<< "<p>" << error_msg << "</p>";
	}
	else
	{
		content << file.rdbuf();
		file.close();
	}
	return (html_template(content.str(), "Error " + code));
}

/*
	* Reads file given a path;
	* If path starts with '/', jumps it;
*/
std::string FileManager::read_file(const std::string path)
{
	std::ifstream file;
	std::stringstream buff;

	file.open(&path.c_str()[path.c_str()[0] == '/']);
	if (!file.is_open())
			throw HttpError("404", "Not Found");
	if (file.is_open())
		buff << file.rdbuf();
	file.close();
	return (buff.str());
}

/*
	* Creates an html file with the list of the files and directories;
	 ! Get route not internal path of the server;

*/
std::string FileManager::directory_listing(const std::string path, const std::string route_path, int port)
{
	std::cout << "path directory: " << path << std::endl;

	DIR* dir = opendir(path.c_str());
	if (!dir)
		throw HttpError("500", "Internal Server Error");
	std::stringstream ss;
	struct dirent* entry;
	entry = readdir(dir);
	ss << "<meta name=\"referrer\" content=\"no-referrer\">";
	ss << "<h1>Directory Listing</h1>";
	ss << "<ul>";
	while (entry)
	{
		std::string entry_path(entry->d_name);
		std::string full_entry_path(path + entry_path);
		ss << "<li>";
		if (Req::get_path_type(full_entry_path) == _DIRECTORY)
		{
			std::cout << "check DIR\n";
			if (entry_path.find_last_of('/') != entry_path.size() - 1)
				entry_path = entry_path + "/";
		}
		ss << "<a href='http://localhost:" << port << "/" << route_path << "/" << entry_path << "'>" << entry_path << "</a>";
		ss << "</li>" << std::endl;
		std::cout << "file name: " <<  path << entry_path << std::endl;
		entry = readdir(dir);
	}
	ss << "</ul>";
	closedir(dir);
	return (html_template(ss.str(), "Directory Listing"));
}

/*
 * Returns the file content;
 * Takes care of the string manipulation;
*/
std::vector<uint8_t> get_file(const std::vector<uint8_t>& content)
{
	std::vector<uint8_t> file;
	size_t pos;
	size_t length;

	pos = RawData::find(content, "\r\n\r\n") + 4;
	if (pos == std::string::npos)
		throw HttpError("400", "Bad Request");
	length = content.size() - pos - 2;
	file = RawData::substr(content, pos, length);
	return (file);
}

/*
 * Returns the status code of the operation;
 * Creates multiple files from POST request;
*/
std::string FileManager::create_files(const std::vector<uint8_t>& body, const std::string & boundary, const std::string dir)
{
	std::vector<std::vector<uint8_t> > files = RawData::split(body, "--" +	 boundary);
	std::ofstream out_file;
	std::string filename;
	std::vector<uint8_t> file;

	for (size_t i = 1; i < files.size() - 1; i++)
	{
		file = get_file(files[i]);
		filename = dir + (dir.empty() || dir[dir.size() - 1] == '/' ? "" : "/") 
					+ get_random_filename(files[i]);
		out_file.open(filename.c_str(), std::ios::binary);
		if (!out_file.is_open())
			throw HttpError("500", "Internal Server error");
		out_file.write((const char*)&file[0], file.size());
		if (out_file.fail())
			throw HttpError("500", "Internal Server error");
		out_file.close();
	}
	return ("201");
}

std::string FileManager::get_random_filename(const std::vector<uint8_t> & file) 
{
	struct timeval		t;
	std::stringstream filename;
	std::string str_filename; 
	std::vector<uint8_t> raw_filename;
	size_t start_pos;
	size_t end_pos;

	gettimeofday(&t, NULL);

	start_pos = RawData::find(file, "filename=\"") + 10;
	end_pos = RawData::find(file, "\"", start_pos);

	filename << (t.tv_sec * 1000) + (t.tv_usec / 1000);
	if (start_pos != std::string::npos && end_pos != std::string::npos)
	{
		raw_filename = RawData::substr(file, start_pos, end_pos - start_pos);
		str_filename = std::string(raw_filename.begin(), raw_filename.end());
		filename << "_" << str_filename;
	}
	return (filename.str());
}
