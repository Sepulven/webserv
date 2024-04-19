#pragma once

#include <Req.hpp>
#include <Res.hpp>

#include <iostream>
#include <string>

class Stream
{
protected:
	std::string time;
public:
	class Req;
	class Res;

	Req req;
	Res res;

	Stream();
	virtual ~Stream();
};

Stream::Stream()
{
}

Stream::~Stream()
{
}
