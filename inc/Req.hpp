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


/*
- receive header and check it
	method + request target + transfer protocol
	host
	user-agent
	accept
	accept-language
	accept-charset
	connection

- check for the end, unchunk if it is chunked
*/

#include <iostream>
#include <ostream>
#include <unistd.h>
#include <map>

class Req
{
	public:
		Req(int connection);
		void	get_request();
		void    map_elements();

	private:
		std::string	line;
		std::map<std::string, std::string>	elements;

		std::string	method;
		std::string	url;
		// std::string version;
};

#endif