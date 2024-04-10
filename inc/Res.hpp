/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Res.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 13:30:36 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/10 13:38:50 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/*
	Response is going to run in a thread, we don't know what it will do.
	May use machines resources.

	Response class responsabilities:
	-> Given the request what should it do?
		-> Execute a CGI
		-> Return a file or a response

	 -> After sending the request;
		* Close the fds;
		* Remove fds from epoll_events array;
		* Update fds in the server vector; 
*/

class Res
{
public:
	Res();
	Res(Res &&) = default;
	Res(const Res &) = default;
	Res &operator=(Res &&) = default;
	Res &operator=(const Res &) = default;
	~Res();

private:
	
};

Res::Res()
{
}

Res::~Res()
{
}