#pragma once

/* C headers*/
#include <stdint.h>
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>

/* C++ headers*/
#include <iostream>
#include <algorithm>
#include <cstring>

/* Classes */
#include <Req.hpp>
#include <HttpError.hpp>
#include <ConnStream.hpp>

enum RES_STATUS {
	OK = 200,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404
};

class ConnStream;

class Res
{
	private:
		ConnStream *stream;

		/* Status */
		std::map<std::string, std::string> status;
		std::map<std::string, std::string> content_type;
 
		int  exec_CGI(void);
		void exec_get(void);
		void exec_post(void);
		void exec_delete(void);

		int build_http_response(void);

		void expand_file_path(void);
		bool validate_route_name(std::string name, std::string filePath);
	public:
		std::string status_code;
		std::string error_msg;
	
		std::string content; // * What is going to get rendered
		std::string data; // * Final Result in case it is not the CGI
		std::string add_ext;

		Res(ConnStream *);
		~Res();

		int send(void);
};
