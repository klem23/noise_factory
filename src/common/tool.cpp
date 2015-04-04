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


#include <cmath>
#include <iostream>

#include "tool.hpp"

void release_mod(note *nt, float* amp_mod_buff, uint32_t release_time, uint32_t nb_sample, uint32_t *end){
	uint32_t shift;

	for(uint32_t p = 0; p < nb_sample; p++){
		amp_mod_buff[p] = 1.0;
	}

	if(nt->stop_offset == 0){
		/*no stop*/
		*end = nb_sample;
	}else{
		/*note off received*/
		if(nt->stop_offset == 1){
			nt->stop_offset = 0;
		}

		/*stop sound in this run*/
		if(nt->stop_offset + release_time - nt->release_offset < nb_sample){
			*end = nt->stop_offset + release_time - nt->release_offset;
			shift = nt->release_offset;

			for(; nt->release_offset < release_time; nt->release_offset++){
				amp_mod_buff[nt->stop_offset + nt->release_offset - shift] =  expf(-10.0 * (float)nt->release_offset / (float)release_time);
			}

			/*to stop*/
			nt->stop_offset = 1;
			nt->toDelete = true;

		}else{
			/*release doesn't finish in this run*/
			*end = nb_sample;
			shift = nt->release_offset;
			for(; nt->release_offset < nb_sample + shift - nt->stop_offset ; nt->release_offset++){
				amp_mod_buff[nt->stop_offset + nt->release_offset - shift] =  expf(-10.0 * (float)nt->release_offset / (float)release_time );
			}
			/*to continue*/
			nt->stop_offset = 1;
		}
	}
}


void smooth_filter(float* in, float* out, int nb_sample){

	uint32_t moy = 4;
	float tmp;

	for(uint32_t i = 0; i < moy; i++){
		out[i] = in[i];
	}

	for(uint32_t i = moy; i < (uint32_t)nb_sample; i++){
		tmp = 0;
		for(uint32_t k = 0; k < moy; k++){
			tmp += in[i - k];
		}
		out[i] = tmp / moy;
	}
}

void smooth_filter_2(float* buff, int nb_sample){
	uint32_t moy = 4;
	float tmp;
	float tmp_data[moy];

	for(uint32_t j = 0; j < moy; j++){
			tmp_data[j] = buff[moy - j - 1];
	}

	for(uint32_t i = moy; i < (uint32_t)nb_sample; i++){

		tmp = 0;
		for(uint32_t k = 1; k <= moy; k++){
			tmp += tmp_data[moy - k];
		}
		buff[i] = tmp / moy;

		for(uint32_t j = moy - 1; j > 0; j--){
			tmp_data[j] = tmp_data[j - 1]; 
		}
		tmp_data[0] = buff[i+1];
	}
}

void smooth_filter(float* buff, int nb_sample){
	uint32_t moy = 4;
	float tmp;
	float tmp_data[moy];

	for(uint32_t j = 0; j < moy; j++){
		tmp_data[j] = buff[j];
	}

	for(uint32_t i = moy; i < (uint32_t)nb_sample; i++){
		tmp = 0;
		for(uint32_t k = 0; k < moy; k++){
			tmp += tmp_data[k];
		}
		buff[i] = tmp / moy;

		for(uint32_t j = 0; j < moy - 1; j++){
			tmp_data[j] = tmp_data[j+1];
		}
		tmp_data[moy - 1] = buff[i+1];
	}
}


void smooth_filter_3(float* buff, int nb_sample){
        uint32_t moy = 5;
        float tmp;
        float tmp_data[2 * moy + 1];

        for(uint32_t j = 0; j < 2 * moy + 1; j++){
                tmp_data[j] = buff[j];
        }

        for(uint32_t i = moy; i < nb_sample - moy; i++){
                tmp = 0;
                for(uint32_t k = 0; k < 2 * moy + 1; k++){
                        tmp += tmp_data[k];
                }
                buff[i] = tmp / 2 * moy + 1;

                for(uint32_t j = 0; j <  2 * moy + 1 ; j++){
                        tmp_data[j] = tmp_data[j+1];
                }
                tmp_data[ 2 * moy ] = buff[i + moy];
        }
}

