#pragma once


/* C headers */
#include <stdint.h>

/* C++ headers */
#include <iostream>
#include <string>

/* Classes */
#include <Req.hpp>
#include <Res.hpp>
#include <ServerContext.hpp>

/*Utils*/
#include <__file_manager_utils.hpp>

class Req;
class Res;

class ConnStream
{
	protected:
		long long last_action;
		long long close_conn_time;
	public:
		ConnStream(int, ServerContext *);
		virtual ~ConnStream();

		int fd;

		Req *req;
		Res *res;

		ServerContext *server;

		void set_time(void);
		void clean_conn();
};
