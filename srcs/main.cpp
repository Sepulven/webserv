/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 12:28:28 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/11 16:17:22 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <WebServer.hpp>

const char* RED_TEXT = "\033[31m";
const char* RESET_COLOR = "\033[0m";

int main(void)
{
	WebServer server;

	try
	{
		server.listen();
	}
	catch (const std::exception& e)
	{
		std::cout << RED_TEXT << "ERROR: " << e.what() << RESET_COLOR << std::endl;
	}
}

