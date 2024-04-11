/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 16:31:58 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/11 23:55:40 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server(std::string _name, int _ip, int _port) 
: name(_name), ip(_ip), port(_port)
{
	this->domain = getaddrinfo(NULL);//...gets the result;
}
Server::~Server()
{
	
}