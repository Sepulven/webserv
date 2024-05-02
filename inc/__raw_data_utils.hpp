#pragma once

/* C++ header */
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>

/* C header */
#include <stdint.h>

namespace RawData
{
	typedef typename std::vector<uint8_t>::iterator iterator;
	typedef typename std::vector<uint8_t>::const_iterator const_iterator;

	size_t find(const std::vector<uint8_t> &, const std::vector<uint8_t> &, size_t);
	size_t find(const std::vector<uint8_t> &, const std::string, size_t);

	void append(std::vector<uint8_t> &, std::vector<uint8_t> &);
	void append(std::vector<uint8_t> &_vec, uint8_t *_suffix, size_t);

	// * Creates a subtrs from base
	std::vector<uint8_t>
	substr(const std::vector<uint8_t> &, size_t, size_t);

	// * Split from string using string to vector of strings
	std::vector<std::string>
	split(const std::string &, const std::string &);

	// * Split from std::vector<uint8_t> to std::vector<uint8_t> using std::vector<uint8_t>
	std::vector<std::vector<uint8_t> >
	split(const std::vector<uint8_t> &, const std::vector<uint8_t> &);

	//* Split from std::vector<uint8_t> to std::vector<uint8_t> using std::string
	std::vector<std::vector<uint8_t> >
	split(const std::vector<uint8_t> &, const std::string);
};
