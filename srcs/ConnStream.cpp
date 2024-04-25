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

void ConnStream::clean_conn()
{
	// * Clean all variable before the new request is received

	// Raw request;
	req->data = "";

	// Request message header
	req->request_line = "";
	req->method = "";
	req->http = "";
	req->URL = "";

	req->body = "";

	// Parse URL data
	req->file_path = "";
	req->filename = "";
	req->file_ext = "";
	req->query_string = "";

	// req->cgi_path = "";

	res->code = "";
	res->content = "";
	res->data = ""; 
}
