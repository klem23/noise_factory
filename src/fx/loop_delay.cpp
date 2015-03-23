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

#include <iostream>
#include <cstring>

#include "loop_delay.hpp"

/*
 *
 * ECHO
 *
 */
echo::echo(uint32_t length, uint32_t rpt)
		:lgth(length),repeat(rpt),r(0),w(0)
		,data(new float[lgth]){

	//data = new float[lgth];
}

echo::~echo(void){
	delete[] data;
}


void echo::write(float* in, uint32_t size){

	
	if(w+size > lgth){
		cout << "echo write error" << endl;	
		return;
	}
	memcpy(data+w, in, size*sizeof(float));
	w += size;
}

void echo::read(float* out, uint32_t size){
	uint32_t cp_size = 0;
	uint32_t total_size = 0;
	//float fb_coeff = pow(feedback/100.0, repeat);

	do{
		if( (lgth - r) >= (size - total_size)){
			cp_size = size - total_size;
		}else{
			cp_size = lgth - r;
		}

		for(uint32_t i = 0; i < cp_size; i++){
			*(out + total_size  + i) += *(data + r  + i); //*feedback
		}
		r += cp_size;
		total_size += cp_size;

		if(r >= lgth){ 
			r = 0;
			repeat--;
		}
	}while((total_size < size )&&(repeat > 0));
}

uint32_t echo::get_lgth_available(void){
	return lgth - w;
}

bool echo::to_delete(void){
	if(repeat <= 0){
		return true;
	}else{
		return false;
	}
}

/*
 *
 * DELAY
 *
 */
loop_delay::loop_delay(uint32_t sample_rate)
		:current_tap(NULL),in(NULL),out(NULL)
		,s_rate(sample_rate),cut_source(false),
		nb_echo(1),delay_s(1),delay_ms(0),feedback(50){

	/*delay_s*s_rate, nb_echo*/
}

loop_delay::~loop_delay(void){
	tap.clear();
}

void loop_delay::set_input(float* ptr){
	in = ptr;
}

void loop_delay::set_output(float* ptr){
		out = ptr;
}

void loop_delay::process(int nb_sample){

	uint32_t nb_write = 0;
	uint32_t delay_shift;
	memset(out, 0, nb_sample * sizeof(float));

	/*copy input if asked*/
	if(cut_source == false){
		memcpy(out, in, nb_sample*sizeof(float));
	}

	/*record echo*/
	if(current_tap == NULL){
		delay_shift = (delay_s + delay_ms/1000.0) * s_rate;
		current_tap = new echo(delay_shift, nb_echo);	
	}

	do{
		uint32_t pos = current_tap->get_lgth_available();
		//		cout << "pos :" << pos << " nb_write :" << nb_write << endl;

		if(pos > (nb_sample - nb_write) ){
			current_tap->write(in + nb_write, nb_sample - nb_write);
			nb_write += (nb_sample - nb_write);
		}else if(pos == (uint32_t)nb_sample){
			current_tap->write(in + nb_write, nb_sample - nb_write);
			tap.push_back(current_tap);
			current_tap = NULL;
			nb_write += (nb_sample - nb_write);
		}else{
			current_tap->write(in + nb_write, pos);
			tap.push_back(current_tap);
			delay_shift = (delay_s + delay_ms/1000.0) * s_rate;
			current_tap = new echo(delay_shift, nb_echo);	
			nb_write += pos;
		}
	}while(nb_write < (uint32_t)nb_sample);

	/*play echo*/
	for(it = tap.begin(); it != tap.end();){
		(*it)->read(out, nb_sample);
		if((*it)->to_delete()){
			delete *it;
			it = tap.erase(it);
		}else{ it++; }
	}
}


void loop_delay::check_param(loop_delay_param *ldp){

	if((uint32_t)*(ldp->nbe) != nb_echo){
		nb_echo = (uint32_t)*(ldp->nbe);
	}

	if((uint32_t)*(ldp->ds) != delay_s){
		delay_s = (uint32_t)*(ldp->ds);
	}

	if((uint32_t)*(ldp->dms) != delay_ms){
		delay_ms = (uint32_t)*(ldp->dms);
	}

	if((uint32_t)*(ldp->fb) != feedback){
		feedback = (uint32_t)*(ldp->fb);
	}
	if((bool)*(ldp->cs) != cut_source){
		cut_source = (uint32_t)*(ldp->cs);
	}
}

