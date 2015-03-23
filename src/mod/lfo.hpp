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

#ifndef LFO_HPP
#define LFO_HPP

#include "mod.hpp"

#define FREQ_LOW  1.00

typedef struct{
	float* active;
	float* type;
	float* freq;
	float* amt;
}lfo_param;

class lfo : public mod {
private:
	uint8_t type;
	float freq;


	float* wave_buff;
	uint32_t s_lgth;

	void calc(void);
	void sinus(void);
	void square(void);
	void saw_up(void);
	void saw_down(void);
	void triangle(void);


public:
	lfo(uint32_t s_rate, bool range_t);
	~lfo(void);

	void check_param(lfo_param* lp);
	void process(float* out, note* nt, uint32_t stop);

};

#endif
