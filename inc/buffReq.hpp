/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Req.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 13:36:32 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/19 12:42:08 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQ_HPP
#define REQ_HPP

#pragma once

/*
	Request is not going to run in a threat;
	It must be build accordingly to the epoll_wait events;

	Request class responsabilities:
	-> Given a write_request call what should it do?
		-> Build it self;
		-> Get the payload;
		-> Check for the end of the request;
		-> For chuncked request it must unchunk it;
	-> Whenever it checks that there is the end of the request it will
		validate the request, get its attr and payload, and instanciate a response;

*/

#include <iostream>
#include <ostream>
#include <unistd.h>
#include <map>
#include <fcntl.h>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <dirent.h>



/*Errors
400 - bad request (request that was sent to the server has invalid syntax)
404 - not found 

401 - unauthorized (user tries to access a resource that is protected by HTTP authentication)
403 - forbidden (lack of permission to access the requested resource,
trying to access a directory that does not have a default index file, and directory listings
are not enabled)
502 - bad gateway (socket doesnt exist)
504 - gateway timeout
*/

class Req
{
	public:
		Req(int connection, std::string line_w);
		void		process_request();
		void		map_elements();
		void		get_info();

		void		send_file();
		void		create_file();
		void		delete_file();

		void		send_response(std::string error_number);
		std::string	get_extension(std::string filename);
		std::string	get_response_body(std::string code);
		void		response_directory();


		// utils
		std::pair<std::string, std::string> split(std::string str, char c);
		std::string	readFile();
		std::string intToString(int value);

	private:
		std::string	line;
		std::string	body;
		std::map<std::string, std::string>	elements;
		int	con;

		std::string	method;
		std::string	location; // file specified in get or delete request
		std::string	connection;
		std::string cont_type;
		std::string cont_len;
		std::string	filename; // file to create (post method)
		std::string	file_to_open; // file to open in get, equivalent to location or index

		// info from config file
		bool		autoindex; // directory listing
		std::string	index; // default file
		std::string	redirect;
		bool		get_allowed;
		bool		post_allowed;
		bool		delete_allowed;
		std::map<std::string, std::string>	error_pages;
};

#endif