/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asepulve <asepulve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 12:39:54 by asepulve          #+#    #+#             */
/*   Updated: 2024/03/27 12:15:43 by asepulve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <BitcoinExchange.hpp>
#include <iostream>
#include <string>
#include <fstream>

/*map*/
int main(int argc, char **argv)
{
	std::ifstream pricesFile;
	std::ifstream bitcoinFile;

	BitcoinExchange bitcoinExchange;

	if (argc == 2 && argv[1])
	{
		pricesFile.open(argv[1]);
		bitcoinFile.open("./data.csv");
	}
	if (argc != 2 || !pricesFile.is_open() || !bitcoinFile.is_open())
	{
		std::cerr << "Error: could not open file." << std::endl;
		return (1);
	}
	bitcoinExchange.setBitcoin(bitcoinFile);
    bitcoinExchange.valueTimesExchangeRate(pricesFile);
	return (0);
}
