/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 16:24:38 by asepulve          #+#    #+#             */
/*   Updated: 2024/04/15 16:24:45 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Error.hpp>

/*Exception class*/
Error::Error(const char *_msg) : msg(_msg) {}

const char * Error::what() const throw()
{
    return (msg);
}
