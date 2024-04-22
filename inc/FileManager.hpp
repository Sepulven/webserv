#pragma once

#include <string>

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
};
