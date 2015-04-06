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

#include <stdint.h>

#include "filter.hpp"

typedef struct{
	float* freq_shift;
	float* volume;
}spf_param;

class spectrum_filter : public filter{
private:
	double* spectrum;
	float* spect_tmp;
	uint32_t s_size;
	uint32_t fscale;

	uint32_t use_point_nb;

	double* d_in;
	double* d_out;

	float amp;
	float freq_shift;
public:

	spectrum_filter(uint32_t s_rate);
	~spectrum_filter(void);

	void process(int nb_sample);
	void set_spectrum(float* val, int nb);
	uint32_t get_table_size();

	void check_param(spf_param* spf_p);

	/*set plug*/
	void set_input(double* input);
	void set_output(double* output);
};
