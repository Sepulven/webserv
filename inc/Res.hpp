#pragma once

/* C headers*/
#include <unistd.h>
#include <cstdlib>
/* C++ headers*/
#include <iostream>
#include <algorithm>

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
		std::string code;
		std::string content; // * What is going to get rendered
		std::string data; // * Final Result

		void exec_get(void);
		void exec_post(void);
		void exec_delete(void);
	public:
		Res(ConnStream *);
		~Res();

		int send(void);
		void log(void) const; // * Logs the the response
};
