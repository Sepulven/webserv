#pragma once

/* C headers*/
#include <unistd.h>

/* C++ headers*/
#include <iostream>
#include <algorithm>

/* Classes */
#include <ConnStream.hpp>

class ConnStream;

class Res
{
	private:
		ConnStream *stream;

		void process_req(void);
		void send_response(std::string);
		void directory_listing(void);

		void send_file(void);
		void create_file(void);
		void delete_file(void);

	public:
		Res(ConnStream *);
		~Res();

		int send(void) const;
};
