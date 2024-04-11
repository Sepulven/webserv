/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 12:31:02 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/11 23:55:12 by asepulve         ###   ########.fr       */
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

	All information about the server context must be hold in server;
	Since we are going to have multiple serve context definition;
*/
class Server
{
	private:		
		std::string	name;
		std::string	ip; // _ip is the string
		int			domain; // Domain is the ip in bits
		int			port;

		int			socket;
	public:
		Server(std::string, int, int);
		virtual ~Server();
};
