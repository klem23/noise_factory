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

#ifndef SPECTRUM_FFT_HPP
#define SPECTRUM_FFT_HPP

#include <stdint.h>
#include <fftw3.h>

using namespace std;

class spectrum_fft{
	protected:
		double *fft_in;
		double *fft_out;
		float* in;
		float *out;
		float coeff;	/* FTT -> FFT-1 => ratio 1/N */

		//bool io;		/* True => input => time to freq // False => output => freq to time */
		uint32_t offset, tmp_size;
		uint32_t table_size;

		fftw_plan fft_plan;
		fftw_r2r_kind kind;
		uint32_t flags;

		void mem_cast_cpy(double* out, float* in, uint32_t size);
		void mem_cast_cpy(float* out, double* in, uint32_t size);


	public:
		spectrum_fft(double* spectrum, uint32_t table_lgth);
		~spectrum_fft(void);

		void set_output(float* ptr);
		void set_input(float* ptr);
};

/*time -> freq*/
class spectrum_fft_in : public spectrum_fft{
	private:


	public:
		spectrum_fft_in(double* spectrum, uint32_t table_lgth);
		~spectrum_fft_in(void);

		void process(int nb_sample);
};

/*freq -> time*/
class spectrum_fft_out : public spectrum_fft{
	private:


	public:
		spectrum_fft_out(double* spectrum, uint32_t table_lgth);
		~spectrum_fft_out(void);

		void process(int nb_sample);
};

#endif
