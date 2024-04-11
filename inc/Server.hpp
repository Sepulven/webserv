/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 12:31:02 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/11 16:32:19 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

/*
	Server contexts:

	All information about the server context must be hold in server;
	Since we are going to have multiple serve context definition;
*/
class Server
{
	private:		
		std::string name;
		int domain;
		int port;
	public:
		Server(std::string, int, int);
		virtual ~Server();
};
