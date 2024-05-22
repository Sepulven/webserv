#include <ConnStream.hpp>

ConnStream::ConnStream(int _fd, ServerContext *_server) : fd(_fd), server(_server)
{
	this->req = new Req(this);
	this->res = new Res(this);
	this->set_time();

	this->cgi_pid = -1;
}

ConnStream::~ConnStream()
{
	delete this->req;
	delete this->res;
	delete this->epoll_event_info;
}

void ConnStream::clean_conn()
{
	// * Clean all variable before the new request is received

	// * Raw request;
	req->data.clear();

	req->content_length = 0;

	// * Request message header
	req->request_line = "";
	req->method = "";
	req->http = "";
	req->URL = "";

	req->raw_body.clear();

	req->header.clear();

	// * Parse URL data
	req->file_path = "";
	req->filename = "";
	req->file_ext = "";
	req->query_string = "";

	req->is_route = "";

	res->status_code = "";
	res->content = "";
	res->data = ""; 
	res->add_ext= ""; 
}

void ConnStream::set_time(void)
{
	struct timeval		t;

	gettimeofday(&t, NULL);
	this->last_action_time = (t.tv_sec * 1000) + (t.tv_usec / 1000);
	this->close_conn_time = this->last_action_time + 20 * 100; // * The connection can last 20 seconds without events;
	this->kill_cgi_time = this->last_action_time + 5 * 100; // * The connection can take 5 seconds to run the CGI
}
