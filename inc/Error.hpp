/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 16:25:39 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/15 16:26:01 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <exception>

class Error : public std::exception
{
	private:
		const char *msg;
		virtual const char * what() const throw();
	public:
		Error(const char *_msg);
};
