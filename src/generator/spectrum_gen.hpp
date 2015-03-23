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

#ifndef SPECTRUM_GEN_HPP
#define SPECTRUM_GEN_HPP

#include "midi_parser.hpp"

//#define TABLE_SIZE 600

class spectrum_gen: public midi_parser{
	private:
		double *spectrum_table;//edit
		double *spectrum_table_tmp;

		float amp;
		float freq_shift;

		uint32_t fscale, table_size;
		uint32_t fscale_table[127];
		double* out;
		

		void start_fill_table(void);		
		void process_audio(int nb_sample);
		void process_audio_2(int nb_sample);


	public:


		spectrum_gen(uint32_t sampling_rate);
		~spectrum_gen(void);

		void set_output(double* ptr);
		void process(int nb_sample);

		void change_signal(uint32_t x, double y);
		void change_signal(float* val, int nb);

		uint32_t get_table_size(void);

		void set_vol(float* vol_t);
		void set_freq_shift(float* freq_shift_t);
};
#endif
