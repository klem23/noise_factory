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


#include <iostream>

#include "conv_filter.hpp"

#include "graph_tool.hpp"

using namespace std;

conv_filter::conv_filter(uint32_t s_rate)
	:filter(s_rate)
	,fscale(10),amp(1){

	//ksize = srate / fscale;
	ksize = 1024;

	kind = FFTW_DHT;
	fft_in = (double*)fftw_malloc( ksize *sizeof(double));
	kernel = (double*)fftw_malloc( ksize *sizeof(double));
	temp_kern = (float*)fftw_malloc( ksize *sizeof(float));

	fft_plan = fftw_plan_r2r_1d(ksize, fft_in, kernel, kind, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);

	
	/*init kernel*/
	uint32_t i = 0;
	do{
		fft_in[i] = 1;
		i++;
	/*
	}while(i < ksize / 4);
	do{
		fft_in[i] = 0;
		i++;*/
	}while(i < ksize);

	/*apply FFT*/
	fftw_execute(fft_plan);


}

conv_filter::~conv_filter(void){
	fftw_destroy_plan(fft_plan);	
	free(fft_in);
	free(kernel);
	free(temp_kern);
}

void conv_filter::set_spectrum(float* val, uint32_t nb){

	//fft process
	memset(fft_in, 0, ksize * sizeof(double));
	memset(kernel, 0, ksize * sizeof(double));
	memset(temp_kern, 0, ksize * sizeof(float));

	/*transpose from GUI graph to plugin table*/
	if(nb != ksize){
		transpose_graph(val, nb, temp_kern, ksize);
	}else{
		memcpy( temp_kern, val, nb * sizeof(float));
	}

	for(uint32_t i = 0; i < ksize; i++){
		fft_in[i] = temp_kern[i];
	}

	fftw_execute(fft_plan);
}	
	

void conv_filter::process(int nb_sample){
	uint32_t max;
	memset(out, 0, nb_sample*sizeof(float));	

	if(kernel != NULL){
		for(uint32_t i = 0; i < (uint32_t)nb_sample; i++){
			if(i < ksize){
				max = i;
			}else{
				max = ksize - 1;
			}

			for(uint32_t k = 0; k <= max; k++){
				out[i] += in[i - k] * kernel[k];
			}
			out[i] = amp * out[i] / ksize;
		}
	}
}

void conv_filter::set_vol(float* vol_t){
	if(vol_t != NULL){
		if(amp != *vol_t){
			amp = *vol_t;
		}
	}
}
