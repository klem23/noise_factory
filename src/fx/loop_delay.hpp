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
#include <vector>

using namespace std;

typedef struct{
        float *cs;
        float *nbe;
        float *ds;
        float *dms;
        float *fb;
}loop_delay_param;


class echo{
	private:
		uint32_t lgth;
		uint32_t repeat;
		uint32_t r, w;
		float* data;

	public:
		echo(uint32_t length, uint32_t rpt);
		~echo(void);

		void write(float* in, uint32_t size);
		void read(float* out, uint32_t size);
		uint32_t get_lgth_available(void);
		bool to_delete(void);
};


class loop_delay{
	private:
		vector<echo*> tap;
		vector<echo*>::iterator it;
		echo* current_tap;

		float *in, *out;

		uint32_t s_rate;

		bool cut_source;
		uint32_t nb_echo;
		uint32_t delay_s;
		uint32_t delay_ms;
		uint32_t feedback;	

	public:
		loop_delay(uint32_t sample_rate);
		~loop_delay(void);

		void check_param(loop_delay_param *ldp);
		void set_input(float* ptr);
		void set_output(float* ptr);
		void process(int nb_sample);
		
};
