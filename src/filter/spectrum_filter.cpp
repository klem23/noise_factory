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

#include "spectrum_filter.hpp"

#include "graph_tool.hpp"

spectrum_filter::spectrum_filter(uint32_t s_rate)
	:filter(s_rate),
	 d_in(NULL),d_out(NULL)
	,amp(1.0),freq_shift(0.0){

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
		cout << "your samplerate is not in standard value 44100 \
			/4800/96000/192000" << endl;
	}

	fscale = srate / s_size;

	cout << "spectrum size for filter : " << s_size << endl;


	spectrum = new double[s_size];
	//spect_tmp = new float[s_size];
	spect_tmp = new float[use_point_nb];

	memset(spectrum, 0, s_size * sizeof(double));
//	memset(spect_tmp, 0, s_size * sizeof(float));
	memset(spect_tmp, 0, use_point_nb * sizeof(float));

	uint32_t i = 0;
	do{
		spectrum[i] = 1;
		spect_tmp[i] = 1;
		i++;
	/*}while(i < s_size/16);
	
	do{
		spectrum[i] = 0;
		spect_tmp[i] = 0;
		i++;*/
	}while(i < use_point_nb);	


}
spectrum_filter::~spectrum_filter(void){
	delete[] spectrum;
	delete[] spect_tmp;

}

void spectrum_filter::process(int nb_sample){
	memset(d_out, 0, s_size*sizeof(double));

/*
	int max = nb_sample;
	if(s_size < nb_sample){
		max = s_size;
	}

*/

	uint32_t decay = (uint32_t)(freq_shift * 10000.0 / fscale);
	uint32_t j = 0;

	//for(int i = 0; i < max; i++){
	for(uint32_t i = 0; i < s_size; i++){
		//d_out[i] = d_in[i] * spectrum[i] * amp;
		if(i - decay < 0){
			j = 0;
		}else if(i - decay >= s_size){
			j = s_size - 1;
		}else{
			j = i - decay;
		}

		d_out[i] = d_in[i] * spectrum[j] * amp;
	}

	
}

void spectrum_filter::set_spectrum(float* val, int nb){

	//transpose_graph(val, nb, spect_tmp, s_size);
	transpose_graph(val, nb, spect_tmp, use_point_nb);
	//transpose_graph_log_to_linear(val, nb, spect_tmp, use_point_nb);
	//for(uint32_t i = 0; i < s_size; i++){
	for(uint32_t i = 0; i < use_point_nb; i++){
		spectrum[i] = spect_tmp[i];
	}
}

uint32_t spectrum_filter::get_table_size(){
	return s_size;
}


/*set plug*/
void spectrum_filter::set_input(double* input){
        	d_in = input;
}

void spectrum_filter::set_output(double* output){
		d_out = output;
}

void spectrum_filter::check_param(spf_param* spf_p){
	if(spf_p->volume != NULL){
		if(amp != *spf_p->volume){
			amp = *spf_p->volume;
		}
	}

	if(spf_p->freq_shift != NULL){
		if(freq_shift != *spf_p->freq_shift){
			freq_shift = *spf_p->freq_shift;
		}
	}
}
