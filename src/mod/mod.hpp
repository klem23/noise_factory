/*
	This file is part of the Noise Factory
	Copyright (C) 2006-2015 Clément Guedez <klem.dev@gmail.com>


    The Noise Factory is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The Noise Factory is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the Noise Factory.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MOD_HPP
#define MOD_HPP

#include <stdint.h>

#include "note.hpp"

using namespace std;

class mod{

private:
	bool active;

protected:
	uint32_t srate;
	bool range; /* false 0 -> 1 for amp ; true -1 -> 1 for freq*/
	float amt; /* mod amplitude*/

public:
	mod(uint32_t s_rate, bool range_p):active(false),srate(s_rate),range(range_p),amt(1.0){}
	//virtual ~mod(void) = 0;

	bool isActive(void) { return active; }
	void activate(void) { active = true; }
	void deactivate(void) { active = false; }

	//virtual void process(float* out, note* nt, uint32_t stop) = 0;
};

#endif
