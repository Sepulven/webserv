/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Req.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 13:36:32 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/19 16:22:52 by asepulve         ###   ########.fr       */
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
		Req();
		~Req();
	
	private:
		// Raw request;
		std::string data;

		// Request message header
		std::string request_line;
		std::string method;
		std::string http;
		std::string URL;
		
		std::map<std::string, std::string> header;

		// Body
		std::string body;

};

#endif