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


#include <cmath>

#include "lfo.hpp"

lfo::lfo(uint32_t s_rate, bool range_t)
	:mod(s_rate, range_t)
	,type(0),freq(1.00){

	s_lgth = srate/freq;

	wave_buff = new float[(uint32_t)(s_rate/FREQ_LOW)];
	calc();
}

lfo::~lfo(void){
	delete[] wave_buff;
}

void lfo::sinus(void){
	if(range){
		float sin_tmp;
		for(uint32_t i = 0; i < s_lgth; i++){
			sin_tmp = amt * sinf(2.0 * M_PI * freq * i / srate);
			wave_buff[i] = powf(2.0, sin_tmp);
		}
	}else{ 
		for(uint32_t i = 0; i < s_lgth; i++){
			wave_buff[i] = amt * sinf(2.0 * M_PI * freq * i / srate) + 1 - amt;
		}
	}
}

void lfo::square(void){
	uint32_t i;

	if(range){

		for(i = 0; i < s_lgth/2; i++){
			wave_buff[i] = powf(2.0, amt);
		}	
		do{
			wave_buff[i] = powf(2.0, - amt);
			i++;
		}while(i < s_lgth);

	}else{
		for(i = 0; i < s_lgth/2; i++){
			wave_buff[i] = 1.0;
		}
		do{
			wave_buff[i] = 1.0 - amt;
			i++;
		}while(i < s_lgth);
	}
}

void lfo::saw_up(void){

	if(range){
		float saw_tmp;
		for(uint32_t i = 0; i < s_lgth; i++){
			saw_tmp = 2.0 * amt / s_lgth * i - amt;
			wave_buff[i] = powf(2.0, saw_tmp);
		}
	}else{
		for(uint32_t i = 0; i < s_lgth; i++){
			wave_buff[i] =  amt / s_lgth * i + (1.0 - amt) ;
		}
	}
}

void lfo::saw_down(void){
	if(range){
		float saw_tmp;
		for(uint32_t i = 0; i < s_lgth; i++){
			saw_tmp = - 2.0 * amt / s_lgth * i + amt;
			wave_buff[i] = powf(2.0, saw_tmp);
		}
	}else{
		for(uint32_t i = 0; i < s_lgth; i++){
			wave_buff[i] = - amt  / s_lgth * i + 1.0;
		}
	}
}

void lfo::triangle(void){
	uint32_t i;

	if(range){

		float tri_tmp;
		for(i = 0; i < s_lgth/2; i++){
			tri_tmp = 4.0 * amt / s_lgth * i - amt;
			wave_buff[i] = powf(2.0, tri_tmp);
		}
		do{
			tri_tmp = - 4.0 * amt/ s_lgth * i + 3 * amt ;
			wave_buff[i] = powf(2.0, tri_tmp);
			i++;
		}while(i < s_lgth);

	}else{

		for(i = 0; i < s_lgth/2; i++){
			wave_buff[i] = 2.0 * amt / s_lgth * i + (1 - amt);
		}

		do{
			wave_buff[i] = - 2.0 * amt / s_lgth * i + 1 + amt;
			i++;
		}while(i < s_lgth);
	}

}


void lfo::calc(void){

	s_lgth =  srate / freq; /* lgth  = time * srate = 1/freq * srate*/

	switch(type){
		case 0: sinus();
			break;
		case 1: square();
			break;
		case 2: saw_up();
			break;
		case 3: saw_down();
			break;
		case 4: triangle();
			break;
	}

}


void lfo::check_param(lfo_param* lp){
	bool refresh = false;

	if(*lp->active){
		activate();
	}else{
		deactivate();
	}

	if((uint8_t)*(lp->type) != type){
		type = (uint8_t)*(lp->type);
		refresh = true;
	}

	if(*lp->freq != freq){
		freq = *lp->freq;
		refresh = true;
	}

	if(*lp->amt != amt){
		amt = *lp->amt;
		refresh = true;
	}

	if(refresh){
		calc();
	}

}


void lfo::process(float* out, note* nt, uint32_t stop){

	uint32_t j = nt->offset % s_lgth;
	uint32_t i = nt->start_offset;

	while(i < stop){
		out[i] *= wave_buff[j];
		i++;
		if(j == s_lgth - 1){
			j = 0;
		}else{
			j++;
		}
	}
}
