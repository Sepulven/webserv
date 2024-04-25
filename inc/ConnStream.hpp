#pragma once


/* C++ headers */
#include <iostream>
#include <string>

/* Classes */
#include <Req.hpp>
#include <Res.hpp>
#include <FileManager.hpp>
#include <ServerContext.hpp>

class Req;
class Res;

class ConnStream
{
	protected:
		std::string time;
	public:
		ConnStream(int, ServerContext *);
		virtual ~ConnStream();

		int fd;

		Req *req;
		Res *res;

		FileManager file;

		ServerContext *server;

		void clean_conn();
};
