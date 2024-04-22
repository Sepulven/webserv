#pragma once


/* C++ headers */
#include <iostream>
#include <string>

/* Classes */
#include <Req.hpp>
#include <Res.hpp>
#include <FileManager.hpp>
#include <Route.hpp>
#include <ServerContext.hpp>

class Req;
class Res;

class ConnStream
{
	protected:
		std::string time;
	public:
		int fd;

		Req *req;
		Res *res;

		FileManager file;
		Route *route;

		ServerContext *server;

		ConnStream(int, ServerContext *);
		virtual ~ConnStream();
};
