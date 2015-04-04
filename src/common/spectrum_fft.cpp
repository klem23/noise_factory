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

#include<cstring>
#include<cstdlib>
#include<cmath>

#include"spectrum_fft.hpp"

spectrum_fft::spectrum_fft(double* spectrum, uint32_t table_lgth)
	:fft_in(NULL),fft_out(NULL),in(NULL),out(NULL)
	,offset(0),tmp_size(0),table_size(table_lgth)
	,fft_plan(NULL),kind(FFTW_DHT){

	coeff = sqrtf((float)table_size);

	//flags = FFTW_ESTIMATE | FFTW_PRESERVE_INPUT; 
	//flags = FFTW_ESTIMATE | FFTW_DESTROY_INPUT; 
	//flags = FFTW_MEASURE | FFTW_PRESERVE_INPUT; 
	flags = FFTW_MEASURE | FFTW_DESTROY_INPUT; 
	//flags = FFTW_PATIENT | FFTW_PRESERVE_INPUT; 
	//flags = FFTW_PATIENT | FFTW_DESTROY_INPUT; 
	//flags = FFTW_EXHAUSTIVE | FFTW_PRESERVE_INPUT; 
	//flags = FFTW_EXHAUSTIVE | FFTW_DESTROY_INPUT; 

}

spectrum_fft::~spectrum_fft(void){
	fftw_destroy_plan(fft_plan);
}

void spectrum_fft::set_input(float* ptr){
	in = ptr;
}

void spectrum_fft::set_output(float* ptr){
	out = ptr;
}

void spectrum_fft::mem_cast_cpy(float* out, double* in, uint32_t size){
	for(uint32_t i = 0; i < size; i++){
		out[i] = (float) in[i]/coeff;
	}
}

void spectrum_fft::mem_cast_cpy(double* out, float* in, uint32_t size){
	for(uint32_t i = 0; i < size; i++){
		out[i] = (double) in[i]/coeff;
	}
}

/*void mem_cast_cpy(T1* in, T2* out, uint32_t size){
	for(uint32_t i = 0; i < size; i++){
		out[i] = (T2)in[i];
	}
}*/


/*
 * time -> freq
 */
spectrum_fft_in::spectrum_fft_in(double* spectrum, uint32_t table_lgth)
	:spectrum_fft(spectrum, table_lgth){


	kind = FFTW_R2HC;
	//kind = FFTW_DHT;
	//kind = FFTW_REDFT00;
	//kind = FFTW_REDFT10;
	//kind = FFTW_REDFT11;
	//kind = FFTW_RODFT00;
	//kind = FFTW_RODFT10;
	//kind = FFTW_RODFT11;

	if(spectrum != NULL){
		/*time -> freq*/
		fft_in = (double*)fftw_malloc( table_size *sizeof(double));
		fft_out = spectrum;

		/*planify fft*/
		fft_plan = fftw_plan_r2r_1d(table_size, fft_in, fft_out, kind, flags);
	}

}

spectrum_fft_in::~spectrum_fft_in(){
	if(fft_in != NULL){
		free(fft_in);
	}
}

void spectrum_fft_in::process(int nb_sample){

	/*time -> freq*/
	if(in == NULL){
		return;
	}

	memset(fft_in, 0, table_size * sizeof(double));
	memset(fft_out, 0, table_size * sizeof(double));


	if(table_size < (uint32_t) nb_sample){
		for(uint32_t i = 0; i < table_size; i++){
			fft_in[i] = (double) in[i]/coeff;
		}
	}else{
		for(uint32_t i = 0; i < (uint32_t)nb_sample; i++){
			fft_in[i] = (double) in[i]/coeff;
		}
	}

	//apply fft
	fftw_execute(fft_plan);
}


/*
 * freq -> time
 */
spectrum_fft_out::spectrum_fft_out(double* spectrum, uint32_t table_lgth)
	:spectrum_fft(spectrum, table_lgth){

	kind = FFTW_HC2R;
	//kind = FFTW_DHT;
	//kind = FFTW_REDFT00;
	//kind = FFTW_REDFT01;
	//kind = FFTW_REDFT11;
	//kind = FFTW_RODFT00;
	//kind = FFTW_RODFT01;
	//kind = FFTW_RODFT11;

	if(spectrum != NULL){
		/*freq -> time*/
		fft_in = spectrum;
		fft_out = (double*)fftw_malloc( table_size *sizeof(double));

		/*planify fft*/
		fft_plan = fftw_plan_r2r_1d(table_size, fft_in, fft_out, kind, flags);
	}

}

spectrum_fft_out::~spectrum_fft_out(){
	if(fft_out != NULL){
		free(fft_out);
	}
}

void spectrum_fft_out::process(int nb_sample){

	/*freq -> time*/
	if(out == NULL){
		return;
	}

	memset(fft_out, 0, table_size * sizeof(double));
	memset(out, 0, nb_sample * sizeof(float));

	//apply fft
	fftw_execute(fft_plan);

	/*VERSION 2*/
	if(table_size > (uint32_t) nb_sample){
		mem_cast_cpy(out, fft_out, nb_sample);
	}else{
		tmp_size = 0;
		while(nb_sample - tmp_size > table_size){
			mem_cast_cpy(out + tmp_size, fft_out, table_size);
			tmp_size += table_size;
		}
		mem_cast_cpy(out + tmp_size, fft_out, nb_sample - tmp_size);
	}
}
