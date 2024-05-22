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

/*Utils*/
#include <__webserv_utils.hpp>

class ConnStream;

class Res
{
	private:
		ConnStream *stream;

		/* Status */
		std::map<std::string, std::string> status;
		std::map<std::string, std::string> content_type;

		int			check_method(void);
		std::string	check_index(void);
		int			check_dir_listing(void);
 
		int  exec_CGI(void);
		void exec_get(void);
		void exec_post(void);
		void exec_delete(void);

		int build_http_response(void);

		std::string	set_file_ext(std::string name);
		
	public:
		std::string status_code;
		std::string error_msg;
	
		std::string content; // * What is going to get rendered
		std::string data; // * Final Result in case it is not the CGI
		std::string add_ext;
		std::string c_type_response;

		Res(ConnStream *);
		~Res();

		int send(void);
};
