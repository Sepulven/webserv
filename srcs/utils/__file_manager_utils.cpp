#include <__file_manager_utils.hpp>


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
	std::string path;
	int error_code = atoi(code.c_str());

	path = error_pages.at(error_code);
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
	DIR* dir = opendir(path.c_str());
	std::stringstream ss;
	struct dirent* entry;
	entry = readdir(dir);

	ss << "<h1>Directory Listing</h1>";
	ss << "<ul>";
	while (entry)
	{
		ss  << "<li>"
			<< "<a href='./" << entry->d_name << "'>" << entry->d_name << "</a>"
			<< "</li>" << std::endl;
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

	size_t pos = RawData::find(content, "\r\n\r\n") + 4;
	size_t length = content.size() - pos - 2;

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
		if (dir[dir.size() - 1] != '/')
			filename = dir + "/" + get_random_filename(files[i]);
		else
			filename = dir + get_random_filename(files[i]);
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
