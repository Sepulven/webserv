#pragma once

#include "Req.hpp"

class Res
{
	private:
	public:
		std::string getTime(void) const;

};


std::string &Res::getTime(void) const {
	return (this->time);
}
