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

#include "env.hpp"

env::env(uint32_t s_rate, bool range_t)
	:mod(s_rate, range_t)
	,A(0),D(0),S(1),R(0){
	
	SA = A * srate;
	SD = D * srate;

	AD_buff = new float[(uint32_t)((A_MAX + D_MAX) * srate)];

	if(range){
		calc_1();
	}else{
		calc_0();
	}
}

env::~env(void){
	delete[] AD_buff;
}

void env::check_param(env_param* ep){
	bool refresh = false;

	if(ep->active == NULL) return;


	if(*ep->active){
		activate();
	}else{
		deactivate();
	}


	if(*ep->AA != A){
		A = *ep->AA;
		refresh = true;
	}

	if(*(ep->DD) != D){
		D = *ep->DD;
		refresh = true;
	}

	if(*ep->SS != S){
		S = *ep->SS;
		refresh = true;
	}

	/*Manage in the generator to produce sounbd long enough
	if(*(ep->RR) != R){
		R = *(ep->RR);
		//refresh = true;
	}
	 */

	/*
	if(*(ep->amt) != amt){
		amt = *(ep->amt);
		refresh = true;
	}
	 */

	if(refresh){
		SA = A * srate;
		SD = D * srate;

		if(range){
			calc_1();
		}else{
			calc_0();
		}
	}
}


/*calc amp 0 -> 1 */
void env::calc_0(void){

	for(int i = 0; i <= SA; i++){
		AD_buff[i] =  amt/SA * i + 1 - amt;
	}

	//float y0 = 1.0 - (S - 1.0)/(SD-SA)*SA
	for(int i = SA; i < SA + SD; i++){
		AD_buff[i] =  (S - 1.0)/SD * (i - SA) + 1.0;
		//AD_buff[i] =  (S - 1.0)/(SD-SA) * i  + 1 - (S - 1.0)/(SD-SA)*SA ;
	}
}

/* calc freq -1 -> 1 */
void env::calc_1(void){
	for(int i = 0; i <= SA; i++){
		AD_buff[i] =  2.0/SA * i - 1.0;
		//AD_buff[i] =  2*amt / SA * i - amt;
		AD_buff[i] /= srate;
		//env_tmp = - amt/SA * i + amt; ok
		//AD_buff[i] = pow(2, env_tmp);
	}

	for(int i = SA; i < SA + SD; i++){
		AD_buff[i] =  (S - 1.0)/SD * (i - SA) + 1.0;
		//AD_buff[i] =  (S - amt)/SD * (i - SA) + amt;
		AD_buff[i] /= srate;
		//env_tmp = 
		//AD_buff[i] = pow(2, env_tmp);
	}

	//S = 0;

}


void env::process(float* out, note* nt, uint32_t stop){
	uint32_t i = nt->start_offset;
	uint32_t j = nt->offset; /*not edit/increment, used by generator*/


	if(!range){
		if( j < SA + SD){
			do{
				out[i] *= AD_buff[j];
				i++;
				j++;
			}while((j < (SA + SD))&&(i < stop));
		}

		while(i < stop){
			out[i] *= S;
			i++;
			j++;
		}
	}else{
		if( j < SA + SD){
			do{
				out[i] += AD_buff[j];
				i++;
				j++;
			}while((j < (SA + SD))&&(i < stop));
		}
	}
}
