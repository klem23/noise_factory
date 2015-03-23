/*
	This file is part of the Noise Factory
	Copyright (C) 2015 Clément Guedez <klem.dev@gmail.com>


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

#ifndef ENV_HPP
#define ENV_HPP

#include "mod.hpp"

using namespace std;

#define A_MAX 1.00
#define D_MAX 1.00
#define R_MAX 1.00

/*Release is implemented in signal generator itself for generating long enough and avoid clipping*/

typedef struct{
	float* active;
	float* AA;
	float* DD;
	float* SS;
	float* RR;
	float* amt;
}env_param;


class env : public mod {
protected:
	/*ADSR value in sec*/
	float A, D, S, R;
	/*ADSR value in sample*/
	float SA, SD; /* SR; in generator for long enough generation*/
	float* AD_buff;
	//float* R_buff;

	void calc_0(void);
	void calc_1(void);

public:
	env(uint32_t s_rate, bool range_t);
	~env(void);

	void check_param(env_param* ep);

	void process(float* out, note* nt, uint32_t stop);
};

#endif
