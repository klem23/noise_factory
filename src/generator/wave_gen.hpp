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

#ifndef WAVE_GEN_HPP
#define WAVE_GEN_HPP

#include <samplerate.h>

#include "midi_parser.hpp"

#define REF_SIZE	600

typedef struct {
	uint32_t x;
	float y;
}coord;

class wave_gen: public midi_parser{
	private:
		float* wave_table[12];
		uint32_t wave_table_size[12];

		float  amp;
		float* amp_mod_buff;

		SRC_DATA src_conf;
		SRC_DATA ref_src_conf;

		float* out;

		void start_fill_table(void);
		void recalculate_table(void);

		void process_audio(int nb_sample);

	public:

		wave_gen(uint32_t sampling_rate);		
		~wave_gen(void);		

		void set_output(float *ptr);
		void process(int nb_sample);

		void change_signal(uint32_t x, float y);
		void change_signal(coord* pos, uint32_t nb);
		void change_signal(float* val, uint32_t nb);

		void set_vol(float* vol_t);
};
#endif
