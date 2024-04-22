#pragma once

/* C headers*/
#include <unistd.h>

/* C++ headers*/
#include <iostream>
#include <algorithm>

/* Classes */
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
		/*Status*/
		std::string& code;
		std::string data; // Response

		/* Set the status code */
		std::map<std::string, std::string> status;
		std::map<std::string, std::string> content_type;

		void process_req(void);
	
		/* New version */
		void exec_get(void);
		void exec_post(void);
		void exec_delete(void);


		std::string get_response_body(void);

		void directory_listing(void);

		void build_response(std::string);
	public:
		Res(ConnStream *);
		~Res();

		int send(void) const;
};
