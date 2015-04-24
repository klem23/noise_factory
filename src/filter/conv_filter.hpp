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

#ifndef CONV_FILTER_HPP
#define CONV_FILTER_HPP


#include <fftw3.h>

#include "filter.hpp"

using namespace std;


class conv_filter: public filter{
	private:
		double*	kernel;
		uint32_t ksize;
		uint32_t fscale;
		float* temp_kern;

		/*FFT process*/
		fftw_plan fft_plan;
		fftw_r2r_kind kind;

		double* fft_in;

		float amp;

	public:
		conv_filter(uint32_t s_rate);
		~conv_filter(void);

		void set_spectrum(float* val, uint32_t nb);
		void process(int nb_sample);

		void set_vol(float* vol_t);
};

#endif
