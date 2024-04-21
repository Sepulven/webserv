#pragma once



#include <iostream>
#include <string>

#include <Req.hpp>
#include <Res.hpp>

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

	ConnStream(int);
	virtual ~ConnStream();
};
