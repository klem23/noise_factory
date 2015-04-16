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

#ifndef PITCH_SHIFTER_HPP
#define PITCH_SHIFTER_HPP

#include <stdint.h>
#include <cstring>
#include <iostream>


typedef struct{
	float* freq_shift;
	float* volume;
}ps_param;

class pitch_shifter{
private:
	uint32_t srate;
	uint32_t s_size;
	float fscale;
	uint32_t use_point_nb;

	double* d_in;
	double* d_out;

	float amp;
	float freq_shift;

public:
	pitch_shifter(uint32_t s_rate);

	void check_param(ps_param* ps);
	void process(int nb_sample);

	/*set plug*/
	void set_input(double* input);
	void set_output(double* output);
	uint32_t get_table_size(void);


};

#endif //PITCH_SHIFTER_HPP

