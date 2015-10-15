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
#include <iostream>

#include "perc.hpp"


perc::perc(uint32_t s_rate)
	:mod(s_rate, 1)
	,freq_env(0.2),amp_env(0.2),freq_sweep(0.5){

}


void perc::check_param(perc_param* pp){

	if(pp->factive == NULL) return;


	if(*pp->factive || *pp->aactive){
		activate();
	}else{
		deactivate();
	}

	if(*pp->factive){
		factive = true;
	}else{
		factive = false;
	}

	if(*pp->aactive){
		aactive = true;
	}else{
		aactive = false;
	}

	if(*pp->fe != freq_env){
		freq_env = *pp->fe;
	}	

	if(*pp->ae != amp_env){
		amp_env = *pp->ae;
	}

	if(*pp->fs != freq_sweep){
		freq_sweep = *pp->fs;
	}
}

void perc::process(float* freq_out, float* amp_out, note* nt, uint32_t stop){
	uint32_t i = nt->start_offset;	
	uint32_t j = nt->offset;	

	float freq_coeff = 1.0/(freq_env * srate);
	float amp_coeff = 1.0/(amp_env * srate);

	/*freq_sweep 0.1 -> 5 * origin freq */
	float freq_tmp;

	while(i < stop){
		//freq_tmp = expf( - j * freq_coeff) * (-freq_sweep) - freq_sweep ;
		//freq_out[i] *= expf( - (float)j * freq_coeff); // + pow(2, freq_tmp) ;
		if(factive){
			if(freq_sweep < 1){
				freq_out[i] *= expf( - (float)j * freq_coeff) * (1 - freq_sweep) + freq_sweep ;
			}else if(freq_sweep > 1){
				freq_out[i] *= - expf( - (float)j * freq_coeff) * freq_sweep + (1 + freq_sweep) ;
			}
		}

		if(aactive) amp_out[i] *= expf( - (float)j  * amp_coeff);
		i++;
		j++;
	}

}
