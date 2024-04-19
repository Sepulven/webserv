#pragma once

#include <Req.hpp>
#include <Res.hpp>

#include <iostream>
#include <string>

class ConnStream
{
protected:
	std::string time;
public:
	class Req;
	class Res;

	Req req;
	Res res;

	ConnStream(int);
	virtual ~ConnStream();
};

ConnStream::ConnStream()
{
}

ConnStream::~ConnStream()
{
}
