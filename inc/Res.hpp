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
	public:
		std::string status_code;
		std::string content; // * What is going to get rendered
		std::string data; // * Final Result
		std::string add_ext;

		Res(ConnStream *);
		~Res();

		int send(void);
};
