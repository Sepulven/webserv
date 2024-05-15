#pragma once

/* C headers */
#include <stdint.h>

/* C++ headers */
#include <iostream>
#include <string>

/* Classes */
#include <Req.hpp>
#include <Res.hpp>
#include <HttpError.hpp>
#include <ServerContext.hpp>

/*Utils*/
#include <__file_manager_utils.hpp>

class Req;
class Res;

class ConnStream
{
	protected:
	public:
		ConnStream(int, ServerContext *);
		virtual ~ConnStream();

		int fd;
		int cgi_pid;

		Req *req;
		Res *res;

		long long last_action_time;
		long long close_conn_time;
		long long kill_cgi_time;

		ServerContext *server;

		void set_time(void);
		void clean_conn();
};
