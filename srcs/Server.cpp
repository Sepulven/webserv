/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ratavare <ratavare@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 16:31:58 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/19 17:32:07 by ratavare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server(std::string _name, std::string _ip, int _port) : name(_name), ip(_ip), port(_port)
{
	this->max_events = 1000;

	// Por enquanto não, mas no futuro sim
		//this->domain = getaddrinfo(NULL);//...gets the result;
	if (this->ip == "localhost")
		this->domain = INADDR_ANY;
}

Server::~Server()
{
	
}
