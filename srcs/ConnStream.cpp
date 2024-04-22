#include <ConnStream.hpp>

ConnStream::ConnStream(int _fd, ServerContext *_server) : fd(_fd), server(_server)
{
	this->req = new Req(this);
	this->res = new Res(this);
}

ConnStream::~ConnStream()
{
	delete this->req;
	delete this->res;
}
