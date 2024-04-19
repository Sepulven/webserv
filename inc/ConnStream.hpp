#pragma once



#include <iostream>
#include <string>

#include <Req.hpp>
#include <Res.hpp>

class ConnStream
{
protected:
	std::string time;
public:
	Req req;
	Res res;

	ConnStream(int);
	virtual ~ConnStream();
};
