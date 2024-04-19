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
	Stream();
	virtual ~Stream();
	class Req;
	class Res;
};

Stream::Stream()
{
}

Stream::~Stream()
{
}
