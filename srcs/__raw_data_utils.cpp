#include <__raw_data_utils.hpp>

/*
 * @param _h(haystack) -> place to look for
 * @param _n(needle) -> pattern to look for
 * Returns the position of when it first found _n in _h from the
 * position _p
 */
size_t RawData::find(const std::vector<uint8_t> &_h, const std::vector<uint8_t> &_n, size_t _p /*= 0*/)
{
	RawData::const_iterator it = std::search(_h.begin() + _p, _h.end(), _n.begin(), _n.end());

	if (it == _h.end())
		return (std::string::npos);
	return (static_cast<size_t>(std::distance(_h.begin(), it)));
}

/*
 * @param _h(haystack) -> place to look for
 * @param _n(needle) -> pattern to look for
 * Returns the position of when it first found _n in _h from the
 * position _p
 */
size_t RawData::find(const std::vector<uint8_t> &_h, std::string _n, size_t _p /*= 0*/)
{
	std::vector<uint8_t> __n(_n.begin(), _n.begin() + _n.length());
	RawData::const_iterator it = std::search(_h.begin() + _p, _h.end(), __n.begin(), __n.end()); // * I don't to compore the \0

	if (it == _h.end())
		return (std::string::npos);
	return (static_cast<size_t>(std::distance(_h.begin(), it)));
}

/*
 * Returns nothing
 * Append the _suffix vector to the of the _vec;
 */
void RawData::append(std::vector<uint8_t> &_vec, std::vector<uint8_t> &_suffix)
{
	_vec.reserve(_vec.size() + _suffix.size());
	_vec.insert(_vec.end(), _suffix.begin(), _suffix.end());
}

/*
 * Returns nothing
 * Append the _suffix arr to the of the _vec;
 */
void RawData::append(std::vector<uint8_t> &_vec, uint8_t *_suffix, size_t _length)
{
	_vec.reserve(_vec.size() + _length);
	_vec.insert(_vec.end(), _suffix, _suffix + _length);
}

/*
 * Returns a new std::vector<uint8_t> from _base starting in the _pos with _length
*/
std::vector<uint8_t> 
RawData::substr(const std::vector<uint8_t> &_base, size_t _pos, size_t _length)
{
	size_t actualLength = std::min(_length, _base.size() - _pos);

	if (_pos >= _base.size())
	    return std::vector<uint8_t>();
	return std::vector<uint8_t>(_base.begin() + _pos, _base.begin() + actualLength);
}

/*
 * Returns a vector of strings
 * Splits the given base string with the delimiter string;
 TODO: Error scenario?
*/
std::vector<std::string> RawData::split(const std::string &base, const std::string &delimiter)
{
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


/*
 * Returns std::vector<std::vector<uint8_t>>
 * Split with base std::vector<uint8_t> delimitador std::vector<uint8_t>
 */
std::vector<std::vector<uint8_t> >
RawData::split(const std::vector<uint8_t> &base, const std::vector<uint8_t> &delimiter)
{
	std::vector<uint8_t> token;
	std::vector<std::vector<uint8_t> > tokens;
	size_t startPos = 0;
	size_t pos = find(base, delimiter, startPos);

	while (pos != std::string::npos)
	{
		token = substr(base, startPos, pos - startPos);
		tokens.push_back(token);

		startPos = pos + delimiter.size();
		pos = find(base, delimiter, startPos);
	}
	tokens.push_back(substr(base, startPos, base.size()));
	return (tokens);
}

/*
 * Returns std::vector<std::vector<uint8_t>>
 * Split with base std::vector<uint8_t> delimitador std::vector<uint8_t>
 */
std::vector<std::vector<uint8_t> >
RawData::split(const std::vector<uint8_t> &base, const std::string delimiter)
{
	std::vector<uint8_t> token;
	std::vector<std::vector<uint8_t> > tokens;
	size_t startPos = 0;
	size_t pos = find(base, delimiter, startPos);

	while (pos != std::string::npos)
	{
		token = substr(base, startPos, pos - startPos);
		tokens.push_back(token);

		startPos = pos + delimiter.size();
		pos = find(base, delimiter, startPos);
	}
	tokens.push_back(substr(base, startPos, base.size()));
	return (tokens);
}

/*
 * Returns std::vector<std::string>
 * Split with base std::vector<uint8_t> delimitador std::string
 TODO: Check whether it is needed or not.
 */
std::vector<std::string>
RawData::splitToString(const std::vector<uint8_t> &base, const std::string delimiter)
{
	std::string token;
	std::vector<uint8_t> __token;
	std::vector<std::string > tokens;
	size_t startPos = 0;
	size_t pos = find(base, delimiter, startPos);

	while (pos != std::string::npos)
	{
		__token = substr(base, startPos, pos - startPos);
		token = std::string(__token.begin(), __token.end());
		tokens.push_back(token);

		startPos = pos + delimiter.size();
		pos = find(base, delimiter, startPos);
	}
	__token = substr(base, startPos, pos - startPos);
	token = std::string(__token.begin(), __token.end());
	tokens.push_back(token);
	return (tokens);
}


void RawData::print_uint(const std::vector<uint8_t> &str)
{
	RawData::const_iterator it = str.begin();
	RawData::const_iterator ite = str.end();

	for (; it != ite; it++)
		std::cout << static_cast<unsigned char>(*it);
}