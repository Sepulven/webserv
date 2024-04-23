#pragma once

/* C++ headers */

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

/* C headers */
#include <sys/stat.h>

/*
	* The only responsability of this class is to deal with the file system;
*/

class FileManager
{
	private:
	public:
		FileManager(/* args */);
		~FileManager();

		static std::string get_extension(const std::string &);
		static std::string get_filename(const std::string &);
		static std::string read_file(const std::string &);
		static std::string create_file(const std::string&, const std::string&);
		static std::string get_path(const std::string&);
};
