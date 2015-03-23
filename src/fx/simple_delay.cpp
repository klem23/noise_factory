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


#include <cstring> 
#include <cmath> 
#include <iostream>

#include "simple_delay.hpp"

simple_delay::simple_delay(uint32_t sample_rate)
	:s_rate(sample_rate),cut_source(false)
	,nb_echo(1),delay_s(1),delay_ms(0),feedback(50)
	,in(NULL),out(NULL){

	
	data_size = NB_ECHO_MAX * DELAY_MAX * s_rate;	
	data = new float[data_size];
	memset(data, 0, data_size * sizeof(float));

	r = data;
	w = data;
	end = data + data_size; 	
}

simple_delay::~simple_delay(void){
	delete[] data;
}

void simple_delay::set_input(float* ptr){
	in = ptr;
}

void simple_delay::set_output(float* ptr){
	out = ptr;
}

void simple_delay::check_param(simple_delay_param *dp){
	
	if((bool)*(dp->cs) != cut_source){
		cut_source = (bool)*(dp->cs);
	}

	if((uint32_t)*(dp->nbe) != nb_echo){
		nb_echo = (uint32_t)*(dp->nbe);
	}

	if((uint32_t)*(dp->ds) != delay_s){
		delay_s = (uint32_t)*(dp->ds);
	}

	if((uint32_t)*(dp->dms) != delay_ms){
		delay_ms = (uint32_t)*(dp->dms);
	}

	if((uint32_t)*(dp->fb) != feedback){
		feedback = (uint32_t)*(dp->fb);
	}

}

void simple_delay::process(int nb_sample){

	float fb_coeff;

	memset(out, 0, nb_sample * sizeof(float));

	uint32_t delay_shift = (delay_s + delay_ms/1000.0) * s_rate;

	/*copy input if asked*/
	if(cut_source == false){
		memcpy(out, in, nb_sample * sizeof(float));
	}

	/*
	 * keep initial write ptr position
	 * for placing read ptr at correct place
	 * before copying echoes
	 */
	float* w2 = w;

	/*write input in delay buffer*/
	/*delay could be slower than latency*/
	for(int j = 0; j < nb_sample; j++){
		if(w == end) w = data;
		*w = in[j];
		w++;
	}

	/*copy echoes*/
	for(uint8_t i = 1; i <= nb_echo; i++){
		fb_coeff = powf(feedback/100.0, i);

		if( w2 - data > i * delay_shift){
			r = w2 - i * delay_shift;
		}else{
			r = end - (i * delay_shift - (w2 - data));
		}

		for(int j = 0; j < nb_sample; j++){
			if( r == end) r = data;
			out[j] += (*r) * fb_coeff;
			r++;
		}
	}



}
