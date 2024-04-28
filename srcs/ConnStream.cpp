#include <ConnStream.hpp>

ConnStream::ConnStream(int _fd, ServerContext *_server) : fd(_fd), server(_server)
{
	this->req = new Req(this);
	this->res = new Res(this);
	this->set_time();
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
	req->data.resize(0);

	// Request message header
	req->request_line = "";
	req->method = "";
	req->http = "";
	req->URL = "";

	req->raw_body.resize(0);

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

void ConnStream::set_time(void)
{
	struct timeval		t;

	gettimeofday(&t, NULL);
	this->last_action = (t.tv_sec * 1000) + (t.tv_usec / 1000);
	this->close_conn_time = this->last_action + 20000; // * The connection can last 20 seconds without events;
}
