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
#include <cstring>

#include "osc.hpp"

#include "tool.hpp"

osc::osc(uint32_t s_rate)
	:midi_parser(s_rate)
	,type(0) ,freq(0) ,phase(0) ,amp(1)
	,release_time(0) ,anticlip_time(0.2 * s_rate)
	,freq_adj(powf(2, freq)) ,out(NULL)
	,freq_mod_buff(NULL),amp_mod_buff(NULL)
	,freq_env(new env(s_rate, true)),freq_lfo(new lfo(s_rate, true))
	,amp_env(new env(s_rate, false)),amp_lfo(new lfo(s_rate, false))
	,perc_env(new perc(s_rate)){

	/*
	freq_adj = powf(2, freq);
	anticlip_time = 0.2 * s_rate;
*/

}

osc::~osc(void){
	delete freq_env;
	delete freq_lfo;
	delete amp_env;
	delete amp_lfo;
	delete perc_env;

	delete[] freq_mod_buff;
	delete[] amp_mod_buff;
}

void osc::set_output(float* ptr){
	out = ptr;
}


void osc::check_param(osc_param *op){

	if((uint8_t)*op->type != type){
		type = (uint8_t)*op->type;
	}

	if(*op->freq != freq){
		freq = *op->freq;
		freq_adj = powf(2, freq);
	}		

	if(*op->phase != phase){
		phase = *op->phase;
	}

	if(*op->amp != amp){
		amp = *op->amp;
	}


	/* release manage by osc to generate long enough sound*/
	if(amp_env->isActive()){
		if(*op->ae.RR != release_time){
			release_time = *op->ae.RR * srate;
		}
	}else{
		release_time = 0;
	}
	//same with frequency ?

	/*mod : ENV/LFO*/
	freq_env->check_param(&(op->fe));
	freq_lfo->check_param(&(op->fl));
	amp_env->check_param(&(op->ae));
	amp_lfo->check_param(&(op->al));

}



/*PRIVATE*/

/*
 * calculate release time shift and associated amp modulation for decreasing vol
 * exp enveloppe is use for avoiding sound clipping
 */
void osc::release_mod(note* nt, uint32_t* end, uint32_t nb_sample){
	uint32_t shift;	

	/*modulation buffer prepare*/
	for(uint32_t p = 0; p < nb_sample; p++){
		amp_mod_buff[p] = 1.0;
		freq_mod_buff[p] = 1.0;
	}

	/* no note off send from now*/
	if(nt->stop_offset == 0){
		*end = nb_sample;
	}else{
		if(nt->stop_offset == 1){
			nt->stop_offset = 0;
		}

		/*release finish in this run*/
		if(nt->stop_offset + release_time + anticlip_time - nt->release_offset < nb_sample){
			*end = nt->stop_offset + release_time + anticlip_time - nt->release_offset;
			shift = nt->release_offset;
			/*Release*/
			for(; nt->release_offset < release_time + anticlip_time; nt->release_offset++){
				//amp_mod_buff[nt->stop_offset + nt->release_offset] =  - 1.0/release_time * nt->release_offset + 1.0;
				amp_mod_buff[nt->stop_offset + nt->release_offset - shift] =  - (float)nt->release_offset / (float)(release_time + anticlip_time) + 1.0;

				/*Anticlip*/
				if(nt->release_offset > release_time){
					//amp_mod_buff[nt->stop_offset + nt->release_offset] =  - 1.0/release_time * nt->release_offset + 1.0;
					amp_mod_buff[nt->stop_offset + nt->release_offset - shift] *=  expf(-10.0 * (float)(nt->release_offset - release_time) / (float)(anticlip_time));
				}
			}
			/*to stop*/
			nt->toDelete = true;
		}else{
			/*release doesn't finish in this run*/
			*end = nb_sample;
			shift = nt->release_offset;
			/*Release*/
			for(; nt->release_offset < nb_sample + shift - nt->stop_offset ; nt->release_offset++){

				amp_mod_buff[nt->stop_offset + nt->release_offset - shift] =  - (float)nt->release_offset / (float)(release_time + anticlip_time) + 1.0;
				/*Anticlip*/
				if(nt->release_offset > release_time){
					//amp_mod_buff[nt->stop_offset + nt->release_offset - shift] =  - 1.0/release_time * nt->release_offset + 1.0;
					amp_mod_buff[nt->stop_offset + nt->release_offset - shift] *=  expf(-10.0 * (float)(nt->release_offset - release_time ) / (float)(anticlip_time));
				}
			}
			/*to continue*/
			nt->stop_offset = 1;
		}
	}
}


/*modulation process in modulation buffer for osc run*/
void osc::modulation(note* nt, uint32_t nb_sample){
	/*modulation process*/
	if(freq_env->isActive()){
		freq_env->process(freq_mod_buff, nt, nb_sample);
	}
	if(freq_lfo->isActive()){
		freq_lfo->process(freq_mod_buff, nt, nb_sample);
	}
	if(amp_env->isActive()){
		amp_env->process(amp_mod_buff, nt, nb_sample);
	}
	if(amp_lfo->isActive()){
		amp_lfo->process(amp_mod_buff, nt, nb_sample);
	}
	if(perc_env->isActive()){
		perc_env->process(freq_mod_buff, amp_mod_buff, nt, nb_sample);
	}
}

/* SINUS */
void osc::sinus(note* nt, uint32_t nb_sample){

	uint32_t end;
	uint32_t i = nt->start_offset;


	//modulation prepare
	release_mod(nt, &end, nb_sample);
	modulation(nt, nb_sample);

	//process audio data
	uint32_t j = nt->offset;
	while(i < end){
		//out[i] += amp * sinf( 2.0 * M_PI * freq_table[nt->key] * j + phase);
		//out[i] += amp * amp_mod_buff[i] * sinf( 2.0 * M_PI * ( freq + freq_table[nt->key] + freq_mod_buff[i] ) * j + phase);
		out[i] += amp * amp_mod_buff[i] * sinf( 2.0 * M_PI * freq_adj * freq_mod_buff[i] * freq_table[nt->key] * j + phase);
		i++;
		j++;
	}


	nt->start_offset = 0;
	nt->offset = j;
}

/* SQUARE */
void osc::square(note* nt, uint32_t nb_sample){

	uint32_t end;
	uint32_t i = nt->start_offset;

	//modulation prepare
	release_mod(nt, &end, nb_sample);
	modulation(nt, nb_sample);

	//process audio data
	uint32_t period[nb_sample];
	for(uint32_t k = 0; k < nb_sample; k++){
		period[k] = (uint32_t) (1.0 / (freq_table[nt->key] * freq_adj * freq_mod_buff[k]));
	}

	uint32_t j = nt->offset % period[0];
	while(i < end){
		if(j > period[i]){
			j = 0;
			out[i] +=  amp * amp_mod_buff[i];
		}else if( j < (period[i] / 2)){
			out[i] +=  amp * amp_mod_buff[i];
		}else{
			out[i] +=  - amp * amp_mod_buff[i];
		}
		i++;
		j++;
	}


	nt->start_offset = 0;
	nt->offset += end - nt->start_offset;
}

/* SAW_UP */
void osc::saw_up(note* nt, uint32_t nb_sample){
	uint32_t end;
	uint32_t i = nt->start_offset;
	float tmp;

	//modulation prepare
	release_mod(nt, &end, nb_sample);
	modulation(nt, nb_sample);

	//process audio data
	uint32_t period[nb_sample];
	for(uint32_t k = i; k < nb_sample; k++){
		period[k] = (uint32_t) (1.0 / (freq_table[nt->key] * freq_adj * freq_mod_buff[k]));
	}
	uint32_t j = nt->offset % period[i];
	//if(j = 0) j += period/4 for starting at 0;
	while( i < end ){
		if(j >= period[i]){
			j = 0;
		}
		tmp = ( 2.0 * (float)j / (float)period[i] ) - 1.0;
		//out[i] *= amp;
		tmp *= amp * amp_mod_buff[i];
		out[i] += tmp;
		i++;
		j++;
	}

	nt->start_offset = 0;
	nt->offset += end - nt->start_offset;
}


/* SAW_DOWN */
void osc::saw_down(note* nt, uint32_t nb_sample){
	uint32_t end;
	uint32_t i = nt->start_offset;
	float tmp;

	//modulation prepare
	release_mod(nt, &end, nb_sample);
	modulation(nt, nb_sample);

	//process audio data
	uint32_t period[nb_sample];
	for(uint32_t k = i; k < nb_sample; k++){
		period[k] = (uint32_t) (1.0 / (freq_table[nt->key] * freq_adj * freq_mod_buff[k]));
	}

	uint32_t j = nt->offset % period[i];
	//if(j = 0) j += period/4 for starting at 0;
	while( i < end ){
		if(j >= period[i]){
			j = 0;
		}
		tmp = - ( 2.0 * (float)j / (float)period[i] ) + 1.0;
		tmp *= amp * amp_mod_buff[i];
		out[i] += tmp;
		i++;
		j++;
	}


	nt->start_offset = 0;
	nt->offset += end - nt->start_offset ;
}

/* TRIANGLE */
void osc::triangle(note* nt, uint32_t nb_sample){
	uint32_t end;
	uint32_t i = nt->start_offset;
	float tmp;


	//modulation prepare
	release_mod(nt, &end, nb_sample);
	modulation(nt, nb_sample);

	//process audio data
	/*
	uint32_t period = (uint32_t) (1.0 / (freq_table[nt->key] + freq));
	uint32_t j = nt->offset % period;
	//if(j = 0) j += period/4 for starting at 0;
        while( i < end ){
                if(j >= period){
                        j = 0;
                        //out[i] = -1;
			out[i] = - amp * amp_mod_buff[i];
		}else if( j < period / 2){
                        out[i] = ( 4 * (float)j / (float)period ) - 1 ;
			//out[i] *= amp;
			out[i] *= amp * amp_mod_buff[i];
                }else{
                        out[i] = - (4 * (float)j / (float)period) + 3 ;
			//out[i] *= amp;
			out[i] *= amp * amp_mod_buff[i];
                }
                i++;
		j++;
        }*/


	uint32_t period[nb_sample];
	for(uint32_t k = i; k < nb_sample; k++){
		period[k] = (uint32_t) (1.0 / (freq_table[nt->key] * freq_adj * freq_mod_buff[k]));
	}

	uint32_t j = nt->offset % period[i];
	//if(j = 0) j += period/4 for starting at 0;
	while( i < end ){
		if(j >= period[i]){
			j = 0;
			//out[i] = -1;
			out[i] += - amp * amp_mod_buff[i];
		}else if( j < period[i] / 2){
			tmp = ( 4.0 * (float)j / (float)period[i] ) - 1.0 ;
			tmp *= amp * amp_mod_buff[i];
			out[i] += tmp;
		}else{
			tmp = - (4.0 * (float)j / (float)period[i]) + 3.0 ;
			tmp *= amp * amp_mod_buff[i];
			out[i] += tmp;
		}
		i++;
		j++;
	}


	nt->start_offset = 0;
	nt->offset += end - nt->start_offset;
}



/*public*/

void osc::process(int nb_sample){
	process_midi(nb_sample);
	process_audio(nb_sample);
}


void osc::process_audio(int nb_sample){
	note nt;

	if(out == NULL){
		return;
	}

	memset(out, 0, nb_sample * sizeof(float));


	if(freq_mod_buff == NULL){
		freq_mod_buff = new float[nb_sample];
	}

	if(amp_mod_buff == NULL){
		amp_mod_buff = new float[nb_sample];
	}


	switch(type){
	/*sin*/
	case 0:
		for(it = note_active.begin(); it != note_active.end();){
			sinus(&(*it), nb_sample);
			if(it->toDelete){
				it = note_active.erase(it);
			}else{
				it++;
			}
			//smooth_filter(out, nb_sample);
		}
		break;
		/*square*/	
	case 1:
		for(it = note_active.begin(); it != note_active.end();){
			square(&(*it), nb_sample);
			if(it->toDelete){
				it = note_active.erase(it);
			}else{
				it++;
			}
			//smooth_filter(out, nb_sample);
		}
		break;	
		/*saw up*/
	case 2:
		for(it = note_active.begin(); it != note_active.end();){
			saw_up(&(*it), nb_sample);
			if(it->toDelete){
				it = note_active.erase(it);
			}else{
				it++;
			}
			//smooth_filter(out, nb_sample);
		}
		break;	
		/*saw down*/
	case 3:
		for(it = note_active.begin(); it != note_active.end();){
			saw_down(&(*it), nb_sample);
			if(it->toDelete){
				it = note_active.erase(it);
			}else{
				it++;
			}
			//smooth_filter(out, nb_sample);
		}
		break;
		/*triangle*/	
	case 4:
		for(it = note_active.begin(); it != note_active.end();){
			triangle(&(*it), nb_sample);
			if(it->toDelete){
				it = note_active.erase(it);
			}else{
				it++;
			}
			//smooth_filter(out, nb_sample);
		}
		break;	
		/*what the fuck happened ?*/
	default:
		cout << "wrong type of osc "<< endl;
		break;	
	}
}


