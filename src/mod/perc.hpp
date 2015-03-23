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

#ifndef PERC_HPP
#define PERC_HPP

#include "mod.hpp"

#define MAX_TIME 2.0;

using namespace std;

typedef struct {
	float *active;
	float *fe;
	float *ae;
	float *fs;
}perc_param;

class perc : public mod{

private:
	float freq_env;
	float amp_env;
	float freq_sweep;

public:
	perc(uint32_t s_rate);

	void check_param(perc_param* pp);
	void process(float* freq_out, float* amp_out, note* nt, uint32_t stop);

};

#endif
