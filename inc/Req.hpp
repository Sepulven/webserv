/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Req.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 13:36:32 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/10 13:43:25 by asepulve         ###   ########.fr       */
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

class Req
{
	public:
		Req(int connection);
		void		process_request();
		void    	map_elements();
		void    	get_info();
		void		send_file();
		void		create_file();
		void		delete_file();
		std::string	readFile(const std::string& filename);
	private:
		std::string	line;
		std::map<std::string, std::string>	elements;
		int	con;

		std::string	method;
		std::string	location;
		std::string	connection;
		std::string cont_type;
		std::string cont_len;

		std::string	file_to_open;
};

#endif