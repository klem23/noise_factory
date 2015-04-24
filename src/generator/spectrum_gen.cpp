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
#include "graph_tool.hpp"

void spectrum_gen::start_fill_table(void){
	spectrum_table[223] = 1;
	spectrum_table[233] = 1;
	spectrum_table[240] = 1;
}

spectrum_gen::spectrum_gen(uint32_t sampling_rate)
	:midi_parser(sampling_rate)
	,amp(1),out(NULL){

	//fscale = srate / TABLE_SIZE;
	fscale = 10;
	table_size = srate / fscale;
	//table_size = 1024;

	use_point_nb = 3072; //18KHz max
	//use_point_nb = 1024;
	//use_point_nb = 8192;


	if(srate == 44100){
		table_size = 7526;
	}else if(srate == 48000){
		table_size = 8192;
	}else if(srate == 96000){
		table_size = 16384;
	}else if(srate == 192000){
		table_size = 32768;
	}else{
		cout << "your samplerate is not in standard value 44100 \
			/4800/96000/192000" << endl;
	}

	fscale = srate / table_size;

	cout << "srate" << srate << " fscale " << fscale << " table_size " << table_size << endl;



	for(uint8_t i = 0; i < 127; i++){
		fscale_table[i] = (int)(freq_table[i] * srate / fscale);
		cout << " n° " << i << " freq_table " << freq_table[i] << " fscale_table " << fscale_table[i] << endl;
	}

	spectrum_table = new double[table_size]; 
	spectrum_table_sum = new double[table_size];
	spectrum_used = new float[use_point_nb];

	memset(spectrum_table, 0, table_size * sizeof(double));
	memset(spectrum_table_sum, 0, table_size * sizeof(double));
	memset(spectrum_used, 0, use_point_nb * sizeof(float));


	start_fill_table();

}

spectrum_gen::~spectrum_gen(void){
	delete[] spectrum_table;
	delete[] spectrum_table_sum;
	delete[] spectrum_used;
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
	memset(spectrum_table_sum, 0, table_size * sizeof(double));

	for(it = note_active.begin(); it != note_active.end(); ){
		//for(int i = 0; i < TABLE_SIZE; i++){
		for(uint32_t i = 0; i < table_size - fscale_table[it->key]; i++){
			//spectrum_table_tmp[i] = spectrum_table[i];
			spectrum_table_sum[i + fscale_table[it->key]] += spectrum_table[i] * amp;
		}


		if(it->stop_offset == 0){
			it++;
		}else{
			it = note_active.erase(it);
		}
	}	

	memcpy(out, spectrum_table_sum, table_size * sizeof(double));

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

void spectrum_gen::change_signal(float* val, uint32_t nb){
	/*uint32_t max = table_size;
	if(nb < (int)table_size){
		max = nb;
	}*/
	memset(spectrum_table, 0, table_size * sizeof(double));
	memset(spectrum_used, 0, use_point_nb * sizeof(float));

	/*transpose GUI graph to plugin table*/
	//graph_adapter::transpose(val, nb, spectrum_table, table_size);
	if(nb != use_point_nb){
		transpose_graph(val, nb, spectrum_used, use_point_nb);
	}else{
		memcpy( spectrum_used, val, nb * sizeof(float));
	}

	for(uint32_t i = 0; i < use_point_nb; i++){
		spectrum_table[i] = spectrum_used[i];
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

