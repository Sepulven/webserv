#include <__raw_data_utils.hpp>

/*
 * @param _h(haystack) -> place to look for
 * @param _n(needle) -> pattern to look for
 * Returns the position of when it first found _n in _h
*/
size_t RawData::find(std::vector<uint8_t>& _h, std::vector<uint8_t>& _n)
{
	iterator it = std::search(_h.begin(), _h.end(), _n.begin(), _n.end());

	if (it == _h.end())
		return (std::string::npos);
	return (static_cast<size_t>(std::distance(_h.begin(), it)));
}

/*
 * @param _h(haystack) -> place to look for
 * @param _n(needle) -> pattern to look for
 * Returns the position of when it first found _n in _h
*/
size_t RawData::find(std::vector<uint8_t>& _h, std::string _n)
{
	iterator it = std::search(_h.begin(), _h.end(), _n.begin(), _n.begin() + _n.length()); // * I don't to compore the \0

	if (it == _h.end())
		return (std::string::npos);
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

/*
 * Returns a vector of strings
 TODO: Error scenario?
*/
std::vector<std::string> split(const std::string& base, const std::string& delimiter)
{
	std::istringstream iss(base);
	std::string token;
	std::vector<std::string> tokens;
	size_t startPos = 0;

	size_t pos = base.find(delimiter, startPos);
	while (pos != std::string::npos)
	{
		token = base.substr(startPos, pos - startPos); 
		tokens.push_back(token);

		startPos = pos + delimiter.length();
		pos = base.find(delimiter, startPos);
	}
	tokens.push_back(base.substr(startPos));
	return tokens;
}

//TODO: Split with base vector delimitador vector retruns std::vector<vector>
//TODO: Split with base vector delimitador string retruns std::vector<string>

//TODO: substring