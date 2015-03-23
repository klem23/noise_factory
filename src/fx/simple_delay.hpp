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

#define NB_ECHO_MAX	5
#define DELAY_MAX	5

typedef struct{
	float *cs;
	float *nbe;
	float *ds;
	float *dms;
	float *fb;
}simple_delay_param;


class simple_delay{
	private:
		uint32_t s_rate;

		bool cut_source;
		uint32_t nb_echo;
		uint32_t delay_s;
		uint32_t delay_ms;
		uint32_t feedback;

		float *data;
		uint32_t data_size;
		float *w, *r, *end;

		float *in, *out;

	public:
		simple_delay(uint32_t sample_rate);
		~simple_delay(void);

		void set_input(float *ptr);
		void set_output(float *ptr);
		void check_param(simple_delay_param *dp);
		void process(int nb_sample);
};
