#pragma once

/* C++ headers */
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <dirent.h>
#include <vector>


/* C headers */
#include <stdint.h>
#include <sys/stat.h>
#include <sys/time.h>

/* Utils */
#include <__raw_data_utils.hpp>

/*
	* The only responsability of this class is to deal with the file system;
	* Reading files;
	* Directory listing;
	* Creating files;
*/

namespace FileManager
{
	// * Get method
	std::string read_file(const std::string);
	std::string directory_listing(const std::string);

	// * Post method
	std::string create_file(const std::vector<uint8_t> &, const std::string);
	std::string create_files(const std::vector<uint8_t> &, const std::string&, const std::string);
	std::string get_random_filename(void) ;
};
