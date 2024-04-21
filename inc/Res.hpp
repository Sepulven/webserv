#pragma once

/* C headers*/
#include <unistd.h>

/* C++ headers*/
#include <iostream>

/* Classes */
#include <ConnStream.hpp>

class ConnStream;

class Res
{
	public:
		Res(ConnStream *);
		~Res();
	
		int send(void) const;
	private:
		ConnStream	*stream;
};
