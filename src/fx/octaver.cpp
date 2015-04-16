/*
	This file is part of the Noise Factory
	Copyright (C) 2015 Clément Guedez <klem.dev@gmail.com>


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

#include "octaver.hpp"


octaver::octaver(uint32_t s_rate)
	:srate(s_rate)
	,d_in(NULL), d_out(NULL)
	,amp(1.0), oct_shift(0){

	//fscale = 10;
	//fscale = 1;
	//s_size = srate / fscale;
	//s_size = 1024;
	//s_size = 8192;
	use_point_nb = 3072; //18KHz max
	//use_point_nb = 1024;
	//use_point_nb = 8192;


	if(srate == 44100){
		s_size = 7526;
	}else if(srate == 48000){
		s_size = 8192;
	}else if(srate == 96000){
		s_size = 16384;
	}else if(srate == 192000){
		s_size = 32768;
	}else{
		std::cout << "your samplerate is not in standard value 44100 \
			/4800/96000/192000" << std::endl;
	}

	fscale = srate / s_size;

}

uint32_t octaver::get_table_size(void){
	return s_size;
}

void octaver::set_input(double* input){
	d_in = input;
}

void octaver::set_output(double* output){
	d_out = output;
}

void octaver::check_param(oct_param* op){

}

void octaver::process(int nb_sample){

	if((d_in == NULL)||(d_out == NULL)){
		return;
	}

	memset(d_out, 0, s_size * sizeof(float));

	for(int i = 0; i < s_size - oct_shift; i++){
		for(int j = 0 ; j < oct_shift; j++){
			d_out[i + j] = d_in[i / oct_shift];
		}
	}
}
