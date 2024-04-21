#include <ConnStream.hpp>

ConnStream::ConnStream(int _fd) : fd(_fd)
{
	this->req = new Req(this);
	this->res = new Res(this);
}

ConnStream::~ConnStream()
{
	delete this->req;
	delete this->res;
}
