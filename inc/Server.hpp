/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 12:31:02 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/21 20:20:15 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/* C headers*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/* C++ headers*/
#include <string>

/*
	Server contexts:

	All information about the server context must be hold in server class;
	Since we are going to have multiple serve context definition;
*/

class Server
{
	private:	

	public:
		std::string	name;
		std::string	ip; // _ip is the string
		int			domain; // Domain is the ip in bits
		int			port;
		int			max_events;
		int			socket;
		
		
		Server(std::string, std::string, int);
		virtual ~Server();
};
