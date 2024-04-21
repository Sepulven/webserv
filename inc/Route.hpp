#pragma once

#include <iostream>
#include <map>

class Route
{
private:
public:
	bool autoindex;	   // directory listing
	std::string index; // default file
	std::string redirect;
	bool get_allowed;
	bool post_allowed;
	bool delete_allowed;

	//Index file;
	std::string index;

	// TODO: Directory in which we should look for in a specific route
	std::map<std::string, std::string> error_pages;
	Route();
	~Route();
};

Route::Route()
{
	// defining elements from config file
	this->autoindex = true;
	this->index = "index.html";
	this->redirect = "";
	this->get_allowed = true;
	this->post_allowed = true;
	this->delete_allowed = true;

	// define default error pages if not specified in file
	this->error_pages["400"] = "./error/400.html";
	this->error_pages["404"] = "./error/404.html";
}

Route::~Route()
{
}
