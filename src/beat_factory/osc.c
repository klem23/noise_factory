/*
	This file is part of the Noise Factory
	Copyright (C) 2009 Clément Guedez <klem.dev@gmail.com>


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

#include "osc.h"

#include <math.h>
#include <stdio.h>

/*anti alias*/
void release_prev(float* out, int stop, float srate){
	int i;
	for(i = 0; i < stop ; i++){
		out[i] *= expf(- i * 200.0 / srate);
	}
}


void freq_env(osc* osci, int nb, float srate, float* data){
	int i, j;
	float env, freq_end, diff;

	j = osci->osc_inc;
	env = 1.0 / (*osci->freq_env * srate);
	diff = *osci->freq_start - *osci->freq_end;
	freq_end = *osci->freq_end;

	for(i = 0 ; i < nb ;i++){
		data[i] = expf( - j * env) * diff + freq_end;
		j++;
	}
}

void amp_env(osc* osci, int nb, float srate, float* data){
	int i, j; 
	float env;

	/*osc already run, so shift already added*/
	j = osci->osc_inc - nb;
	//j = osci->osc_inc;
	env = 1.0 / (*osci->amp_env * srate);

	for(i = 0 ; i < nb ;i++){
		data[i] = expf( - j * env) * data[i];
		j++;
	}
}

void osc_sin(osc* osci, int nb, float srate, float* data, float* out){
	int i, j;
	//float freq;
	float vol = *osci->vol;

	j = osci->osc_inc;
	for(i = 0 ; i < nb ;i++){
		//data[i] = sinf( 2 * M_PI * freq * j / srate );
		//out[i] = vol * sinf( 2.0 * M_PI * data[i] * j / srate );
		out[i] = vol * sinf( 2.0 * 3.1416 * data[i] * j / srate );
		j++;
	}
	osci->osc_inc += nb;
}

void osc_square(osc* osci, int nb, float srate, float* data, float* out){
	int i, j, period[nb];
	float vol = *osci->vol;

	j = osci->osc_inc;

	for(i = 0 ; i < nb ; i++){
		period[i] = (int) ( srate / data[i] );
		if( j % period[i] < period[i] / 2.0){
			out[i] = vol;
		}else{
			out[i] = - vol;
		}
		j++;
	}
	osci->osc_inc += nb;
}

void osc_saw_up(osc* osci, int nb, float srate, float* data, float* out){
	int i, tmp, period[nb];
	float vol = *osci->vol;

	period[0] = (int) ( srate / data[0] );
	tmp = osci->osc_inc % period[0] ;

	for(i = 0 ; i < nb ;i++){
		period[i] = (int) ( srate / data[i] );
		if(tmp >= period[i]){
			tmp = 0;
		}
		out[i] = (2.0 * tmp / period[i] - 1.0) * vol ;
		tmp++;
	}
	osci->osc_inc += nb;
}

void osc_saw_down(osc* osci, int nb, float srate, float* data, float* out){
	int i, tmp, period[nb];
	float vol = *osci->vol;

	period[0] = (int) ( srate / data[0] );
	tmp = osci->osc_inc % period[0] ;

	for(i = 0 ; i < nb ;i++){
		period[i] = (int) ( srate / data[i] );
		if(tmp >= period[i]){
			tmp = 0;
		}
		out[i] =  ( - 2.0 * tmp / period[i] + 1.0) * vol ;
		tmp++;
	}
	osci->osc_inc += nb;
}

void osc_triangle(osc* osci, int nb, float srate, float* data, float* out){
	int i, tmp, period[nb];
	float vol = *osci->vol;

	period[0] = (int) ( srate / data[0] );
	tmp = osci->osc_inc % period[0] ;

	for(i = 0 ; i < nb ;i++){
		period[i] = (int) ( srate / data[i] );
		if( tmp < period[i] / 2.0){
			out[i] = (4.0 * tmp / period[i] - 1.0) * vol ;
		}else if(tmp >= period[i]){
			tmp = 0;
			out[i] = - vol;
		}else{
			out[i] = ( - 4.0 * tmp / period[i] + 3.0) * vol ;
		}
		tmp++;
	}
	osci->osc_inc += nb;
}
