#include <Route.hpp>

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