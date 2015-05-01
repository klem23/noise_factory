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

#ifndef FILTER_HPP
#define FILTER_HPP

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdint.h>

using namespace std;

#define	LOW_PASS	0
#define	HIGH_PASS	1
#define	BAND_PASS	2
#define	BAND_STOP	3


class filter{
	private:
	public:
		enum band_type_e{
			LOW_PASS_e,
			HIGH_PASS_e,
			BAND_PASS_e,
			BAND_STOP_e
		};		


		uint32_t srate;
		uint8_t band_type;
		uint8_t order;
		float fc;
		float q;
		
		float *in, *out;
		
		filter(uint32_t s_rate):srate(s_rate),band_type(LOW_PASS),order(2),
								fc(0.1),q(0.5),in(NULL),out(NULL){};
		~filter(void){};

		/*set plug*/
		void set_input(float* input){ in = input;}
		void set_output(float* output){ out = output; }

		/*set parameter*/
		void f_set_order(uint8_t order_t){
			if(order_t > 0){
				order = order_t;
			}
		}
		void f_set_fc(float cutoff){
			if(cutoff <= (float)srate/2 ){
				fc = cutoff/srate;
			}
		}

		float get_fc(void){ return fc * srate; }

		void f_set_band_type(uint8_t type){
			if(type < 4){
				band_type = type;
			}
		}

		//void process(nb_sample);
};

class recursive_filter : public filter{
	protected:
		double*	a_coeff;
		double*  b_coeff;

		double* in_cache;
		double* out_cache;

	public:
		recursive_filter(uint32_t s_rate);
		~recursive_filter(void);


		void rf_set_order(uint8_t order_t);
		void process(int nb_sample);


};

#endif
