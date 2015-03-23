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

#include "fenv.hpp"

fenv::fenv(uint32_t s_rate)
	:env(s_rate, 1)
	,A_UP(true),D_ON(false),R_UP(false){

}

fenv::~fenv(void){
	delete[] AD_buff;
}

void fenv::check_param(fenv_param* fp){
	bool refresh = false;


	if(*fp->active){
		activate();
	}else{
		deactivate();
	}


	if(*fp->A_time != A){
		A = *fp->A_time;
		refresh = true;
	}

	if(*(fp->D_time) != D){
		D = *fp->D_time;
		refresh = true;
	}

	/*Release is manage in generator
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

		calc();

	}
}
