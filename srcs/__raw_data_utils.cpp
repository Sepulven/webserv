#include <__raw_data_utils.hpp>

/*
 * @param _h(haystack) -> place to look for
 * @param _n(needle) -> pattern to look for
 * Returns the position of when it first found the _n in _h
*/
size_t RawData::find(std::vector<uint8_t>& _h, std::vector<uint8_t>& _n)
{
	iterator it = std::search(_h.begin(), _h.end(), _n.begin(), _n.end());

	return (static_cast<size_t>(std::distance(_h.begin(), it)));
}

/*
 * @param _h(haystack) -> place to look for
 * @param _n(needle) -> pattern to look for
 * Returns true if the _n string is found inside the _h vector;
*/
size_t RawData::find(std::vector<uint8_t>& _h, std::string _n)
{
	iterator it = std::search(_h.begin(), _h.end(), _n.begin(), _n.begin() + _n.length()); // * I don't to compore the \0

	return (static_cast<size_t>(std::distance(_h.begin(), it)));
}

/*
 * Returns nothing
 * Append the _suffix vector to the of the _vec;
*/
void RawData::append(std::vector<uint8_t>& _vec, std::vector<uint8_t>& _suffix)
{
	_vec.reserve(_vec.size() + _suffix.size());
	_vec.insert(_vec.end(), _suffix.begin(), _suffix.end());
}

/*
 * Returns nothing
 * Append the _suffix arr to the of the _vec;
*/
void RawData::append(std::vector<uint8_t>& _vec, uint8_t* _suffix, size_t _length)
{
	_vec.reserve(_vec.size() + _length);
	_vec.insert(_vec.size(), _suffix, _suffix + _length);
}
