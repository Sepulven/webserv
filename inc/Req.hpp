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

/* Utils */
#include <__raw_data_utils.hpp>

enum PATH_TYPE
{
	_DIRECTORY = 'd',
	_FILE = 'f',
	_NONE = 'n'
};

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

		std::map<std::string, std::string> header;

		// * Body
		std::size_t content_length;
		std::vector<uint8_t> raw_body;
		std::vector<uint8_t> chunk;
		int chunk_length;

		// * Parse URL data
		std::string file_path;
		std::string filename;
		std::string file_ext;
		std::string query_string;

		std::string cgi_path;

		enum PATH_TYPE path_type;

		int read(int);
		void set_raw_body(size_t);
		void set_URL_data(std::string &);
		void set_header(std::vector<std::string> &);
		void parser(void);
		void unchunk(const uint8_t*, size_t);
		void get_last_chunk(const std::vector<uint8_t>&);
		void log(void) const;
};
