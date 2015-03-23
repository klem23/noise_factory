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
	public:
		float*	a_coeff;
		float*  b_coeff;

		float* in_cache;
		float* out_cache;

		recursive_filter(uint32_t s_rate)
			:filter(s_rate)
			,a_coeff(NULL),b_coeff(NULL)
			,in_cache(NULL),out_cache(NULL){

			rf_set_order(2);
		};

		~recursive_filter(void){
				delete a_coeff;
				delete b_coeff;
				delete in_cache;
				delete out_cache;
		};

		void rf_set_order(uint8_t order_t){
			if(order_t > 0){	
				order = order_t;

				delete a_coeff;
				delete b_coeff;
				delete in_cache;
				delete out_cache;

				a_coeff = new float[order_t + 1];
				b_coeff = new float[order_t + 1];
				in_cache = new float[order_t + 1];
				out_cache = new float[order_t + 1];

				if((a_coeff != NULL)&&(b_coeff != NULL)){
					memset(a_coeff, 0, (order_t + 1)*sizeof(float));
					memset(b_coeff, 0, (order_t + 1)*sizeof(float));
					memset(in_cache, 0, (order_t + 1)*sizeof(float));
					memset(out_cache, 0, (order_t + 1)*sizeof(float));
					//B0 doesn't exist normally
					b_coeff[0] = 0.0;
					for(int i = 0; i <= order_t; i++){
						in_cache[i] = 0.0;
						out_cache[i] = 0.0;
					}					
				}
			}
		}

		void process(int nb_sample){
			memset(out, 0, nb_sample*sizeof(float));
			//a problem happend when calculing the coefficient
			if((a_coeff == NULL)||(b_coeff == NULL)||(a_coeff[0] == 0.0)){
				return;
			}
		
			//calculus from data cache of previous run
			for(int i = 0; i < order; i++){
				in_cache[0] = in[i];
				out_cache[0] = 0;

				for(int j = order; j >= 0 ; j--){
					out[i] += a_coeff[j] * in_cache[j] + b_coeff[j] * out_cache[j];

					if(j != 0){
						in_cache[j] = in_cache[j-1];
						out_cache[j] = out_cache[j-1];
					}
				}
				out_cache[1] = out[i];
			}

			//real calculus for full run
			for(int i = order; i < nb_sample; i++){
				for(int j = 0; j <= order ; j++){
					out[i] += a_coeff[j] * in[i-j] + b_coeff[j] * out[i-j];
				}
			}
		
			//caching data for next run
			for(int i = 1; i <= order; i++){
				in_cache[i] = in[nb_sample - i];
				out_cache[i] = out[nb_sample - i];
			}
		}

};

#endif
