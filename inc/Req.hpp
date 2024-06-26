#pragma once

/*
	Request is not going to run in a threat;
	It must be build accordingly to the epoll_wait events;

	Request class responsabilities:
	-> Given a write_request call what should it do?
		-> Build it self;
		-> Get the payload;
		-> Check for the end of the request;
		-> For chuncked request it must unchunk it;
	-> Whenever it checks that there is the end of the request it will
		validate the request, get its attr and payload, and instanciate a response;
*/

/* C++ header */
#include <iostream>
#include <map>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include <filesystem>

/* C header */
#include <stdint.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

/* Classes */
#include <ConnStream.hpp>
#include <HttpError.hpp>

/* Utils */
#include <__raw_data_utils.hpp>
#include <__webserv_utils.hpp>

class ConnStream;

class Req
{
	private:
	public:
		Req(ConnStream *);
		~Req();

		ConnStream *stream;
		
		const size_t out_of_bound; // * An alias of std::string::npos

		// * Raw request;
		std::vector<uint8_t> data;

		// * Request message header
		std::string request_line;
		std::string method;
		std::string http;
		std::string URL;
		std::string referer;
		std::string cookie;

		std::map<std::string, std::string> header;

		// * Body
		std::size_t content_length;
		std::vector<uint8_t> raw_body;

		// * Parse URL data
		std::string file_path;
		std::string route_path;
		std::string filename;
		std::string file_ext;
		std::string query_string;

		std::string is_route;
		int			route_id;

		enum PATH_TYPE path_type;

		// * Path expansion based on route name.
		void expand_file_path();
		bool validate_route_name(std::string, std::string);

		// * I think this will be useful.
		static enum PATH_TYPE get_path_type(std::string);

		int read(int);
		// * Parsing.
		void set_raw_body(size_t);
		void set_URL_data(std::string &);
		void set_header(std::vector<std::string> &);
		void set_file_ext(void);
		void set_content_length(void);
		void set_rest_raw_data(size_t);
		void set_referer(std::vector<std::string>);
		void set_cookie(std::vector<std::string> message_header);
		void parser(size_t);
};
