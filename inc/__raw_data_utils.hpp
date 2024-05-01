#pragma once

/* C++ header */
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>


/* C header */
#include <stdint.h>

namespace RawData
{
	typedef typename std::vector<uint8_t>::iterator iterator;

	size_t find(std::vector<uint8_t>&, std::vector<uint8_t>&);
	size_t find(std::vector<uint8_t>&, std::string);
	void append(std::vector<uint8_t>&, std::vector<uint8_t>&);
	void append(std::vector<uint8_t>& _vec, uint8_t* _suffix, size_t);
};
