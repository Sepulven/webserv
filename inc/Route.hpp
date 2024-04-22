#pragma once

#include <iostream>
#include <map>

class Route
{
	private:
		bool autoindex;	   // directory listing
		std::string index; // default file
		std::string redirect;
		bool get_allowed;
		bool post_allowed;
		bool delete_allowed;

		std::string index_file;
		std::map<std::string, std::string> error_pages;

	public:
		// TODO: Directory in which we should look for in a specific Route
		Route();
		~Route();
};
