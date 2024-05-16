#pragma once

/* C++ headers */
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <dirent.h>
#include <vector>
#include <map>

/* C headers */
#include <stdint.h>
#include <sys/stat.h>
#include <sys/time.h>

/* Utils */
#include <__raw_data_utils.hpp>

/* Classes */
#include <HttpError.hpp>


/*
	* The only responsability of this class is to deal with the file system;
	* Reading files;
	* Directory listing;
	* Creating files;
	* Biuilding error pages;
*/

namespace FileManager
{
	// * Error cases;
	std::string build_error_pages(const std::map<int, std::string> &, const std::string &, const std::string &);

	// * Get method
	std::string read_file(const std::string);
	std::string directory_listing(const std::string);

	// * Post method
	std::string create_files(const std::vector<uint8_t> &, const std::string&, const std::string);
	std::string get_random_filename(const std::vector<uint8_t>&);
};
