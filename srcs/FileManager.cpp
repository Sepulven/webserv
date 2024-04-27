#include <FileManager.hpp>

static std::vector<std::basic_string<uint8_t> > split(const std::basic_string<uint8_t>& base, const std::string& _d);

FileManager::FileManager() {}

//* Utils
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


// * Utils
static std::vector<std::basic_string<uint8_t> > split(const std::basic_string<uint8_t>& base, const std::string& _d)
{
	std::basic_string<uint8_t> token;
	std::basic_string<uint8_t> delimitador(_d.begin(), _d.end());
	std::vector<std::basic_string<uint8_t> > tokens;
	size_t startPos = 0;

	size_t pos = base.find(delimitador, startPos);
	while (pos != std::string::npos)
	{
		token = base.substr(startPos, pos - startPos); 
		tokens.push_back(token);

		startPos = pos + delimitador.length();
		pos = base.find(delimitador, startPos);
	}
	tokens.push_back(base.substr(startPos));
	return tokens;
}

// static void print_uint(const std::basic_string<uint8_t> &str)
// {
// 	std::basic_string<uint8_t>::const_iterator it = str.begin();
// 	std::basic_string<uint8_t>::const_iterator ite = str.end();

// 	for (; it != ite; it++)
// 		std::cout << static_cast<unsigned char>(*it);
// }

/*
	* Don't add the extension, as we can't figure out what is the extension;
	* In case the file creation 
*/
std::string FileManager::create_files(const std::basic_string<uint8_t>& body, const std::string & boundary, const std::string dir)
{
	std::vector<std::basic_string<uint8_t> > files = split(body, "--" +	 boundary);
	uint8_t clrf[] = {'\r', '\n', '\r', '\n'};
	std::basic_string<uint8_t> pattern(clrf, 4);
	std::basic_string<uint8_t> file;
	std::ofstream out_file;
	std::string filename;

	for (size_t i = 1; i < files.size() - 1; i++)
	{
		file = files[i].substr(files[i].find(pattern) + pattern.length());
		filename = dir + "/" + get_random_filename() + (char)(i+48);

		out_file.open(filename.c_str(), std::ios::binary);
		if (!out_file.is_open())
			return ("500");
		out_file.write((const char*)&file[0], file.length());
		if (out_file.fail())
			return ("500");
		out_file.close();
	}
	return ("201");
}

std::string get_random_filename(void) 
{
	struct timeval		t;
	std::stringstream filename;

	gettimeofday(&t, NULL);
	filename << (t.tv_sec * 1000) + (t.tv_usec / 1000);
	return (filename.str());
}

