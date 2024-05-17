/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContext.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 12:31:02 by asepulve          #+#    #+#             */
/*   Updated: 2024/05/15 01:13:29 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/* C headers*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/* C++ headers*/
#include <string>
#include <vector>

/* Classes */
#include "Parser.hpp"

/*
	Server contexts:

	All information about the server context must be hold in server class;
	Since we are going to have multiple serve context definition;
*/

typedef struct s_location
{
	std::string name;
	std::string	root;
	std::string	redirect;
	std::vector<std::string>	http_methods;
	std::vector<std::string>	index;
	bool	dir_listing;
} t_location;


class ServerContext
{z
	private:

	public:
		std::string	name;
		std::string	ip; // _ip is the string
		int			domain; // Domain is the ip in bits
		int			port;
		int			max_events;
		int			socket;
		long long	max_cb_size;

		std::map<int, std::string> error_pages;
		std::vector<t_location> routes;

		ServerContext(t_server serverNode);
		virtual ~ServerContext();
};
