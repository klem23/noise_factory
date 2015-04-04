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
#include <cstdlib>
#include <cstring>

#include "spectrum_gen.hpp"

void spectrum_gen::start_fill_table(void){
	spectrum_table[23] = 1;
	//spectrum_table[24] = 1;
	//spectrum_table[233] = 1;
}

spectrum_gen::spectrum_gen(uint32_t sampling_rate)
	:midi_parser(sampling_rate)
	,amp(1),freq_shift(0),out(NULL){

	//fscale = srate / TABLE_SIZE;
	fscale = 10;
	table_size = srate / fscale;
	//table_size = 1024;
	cout << "srate" << srate << " fscale " << fscale << " table_size " << table_size << endl;

	for(uint8_t i = 0; i < 127; i++){
		fscale_table[i] = (int)(freq_table[i] * srate / fscale);
		cout << " n° " << i << " freq_table " << freq_table[i] << " fscale_table " << fscale_table[i] << endl;
	}

	spectrum_table = new double[table_size]; 
	spectrum_table_tmp = new double[table_size]; 

	memset(spectrum_table, 0, table_size * sizeof(double));
	memset(spectrum_table_tmp, 0, table_size * sizeof(double));


	start_fill_table();

}

spectrum_gen::~spectrum_gen(void){
	delete[] spectrum_table;
	delete[] spectrum_table_tmp;
}

void spectrum_gen::set_output(double * ptr){
		out = ptr;
}

void spectrum_gen::process(int nb_sample){
	process_midi(nb_sample);
	process_audio(nb_sample);
}

void spectrum_gen::process_audio(int nb_sample){

	if(out == NULL){
		return;
	}

	memset(out, 0, table_size * sizeof(double));
	memset(spectrum_table_tmp, 0, table_size * sizeof(double));

	for(it = note_active.begin(); it != note_active.end(); ){
		//for(int i = 0; i < TABLE_SIZE; i++){
		for(uint32_t i = 0; i < table_size - fscale_table[it->key]; i++){
			//spectrum_table_tmp[i] = spectrum_table[i];
			spectrum_table_tmp[i + fscale_table[it->key]] += spectrum_table[i] * amp;
		}


		if(it->stop_offset == 0){
			it++;
		}else{
			it = note_active.erase(it);
		}
	}	

	memcpy(out, spectrum_table_tmp, table_size * sizeof(double));

}
/*
void spectrum_gen::process_audio_2(int nb_sample){
	uint32_t tmp_size;

	for(it = note_active.begin(); it != note_active.end(); it++){

		if(TABLE_SIZE - (*it).offset > (uint32_t)nb_sample){
			memcpy(out, spectrum_table + (*it).offset*sizeof(float), nb_sample*sizeof(float));
		}else{
			memcpy(out, spectrum_table + (*it).offset*sizeof(float), (TABLE_SIZE - (*it).offset)*sizeof(float));
			tmp_size = TABLE_SIZE - (*it).offset;
			while(nb_sample - tmp_size > TABLE_SIZE){	
				memcpy(out + tmp_size*sizeof(float), spectrum_table, TABLE_SIZE*sizeof(float));
				tmp_size += TABLE_SIZE;
			}
			memcpy(out + tmp_size*sizeof(float), spectrum_table, (nb_sample - tmp_size)*sizeof(float));
		}	

		(*it).offset += nb_sample;
		(*it).offset = (*it).offset % TABLE_SIZE;

	}
}
*/

void spectrum_gen::change_signal(uint32_t x, double y){
	spectrum_table[x] = y;
}

void spectrum_gen::change_signal(float* val, int nb){
	uint32_t max = table_size;
	if(nb < (int)table_size){
		max = nb;
	}

	/*transpose GUI graph to plugin table*/
	//graph_adapter::transpose(val, nb, spectrum_table, table_size);

	for(uint32_t i = 0; i < max; i++){
		spectrum_table[i] = val[i];
	}
}


uint32_t spectrum_gen::get_table_size(void){
	return table_size;
}

void spectrum_gen::set_vol(float* vol_t){
	if(vol_t != NULL){
		if(amp != *vol_t){
			amp = *vol_t;
		}
	}
}

void spectrum_gen::set_freq_shift(float* freq_shift_t){
	if(freq_shift_t != NULL){
		if(freq_shift != *freq_shift_t){
			freq_shift = *freq_shift_t;
		}
	}
}

