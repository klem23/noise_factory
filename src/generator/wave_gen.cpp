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
#include <iostream>
#include <cmath>

#include "wave_gen.hpp"

#include "tool.hpp"
#include "graph_tool.hpp"



void wave_gen::start_fill_table(void){

	for(uint32_t i = 0; i < wave_table_size[0]; i++){
		//wave_table[0][i] = sin( 2 * M_PI * 85 * i / srate);
		wave_table[0][i] = sin( 2 * M_PI * 8.176 * i / srate);
	}
	recalculate_table();
}


wave_gen::wave_gen(uint32_t sampling_rate)
	:midi_parser(sampling_rate)
	,amp(1),amp_mod_buff(NULL),out(NULL){

	/*preparing samplerate for complete octave*/
	for(int i = 0; i < 12; i++){
		wave_table_size[i] = 1 / freq_table[i];
		//wave_table[i] = (float*)malloc(wave_table_size[i] * sizeof(float));
		wave_table[i] = new float[wave_table_size[i]];
		cout << "table size" << i << " : " <<wave_table_size[i] << endl;
	}

	src_conf.data_in = wave_table[0];
	src_conf.input_frames = wave_table_size[0];

	start_fill_table();
}


wave_gen::~wave_gen(void){
	for(int i = 0; i < 12; i++){
		if(wave_table[i] != NULL){
			//free(wave_table[i]);
			delete wave_table[i];
		}
	}
}

void wave_gen::set_output(float* ptr){
		out = ptr;
}

void wave_gen::process(int nb_sample)
{
	process_midi(nb_sample);
	process_audio(nb_sample);
}

void wave_gen::process_audio(int nb_sample){
	uint8_t octave, note_val, shift;
	uint32_t i, j, end;

	if(out == NULL){
		return;
	}

	memset(out, 0, nb_sample * sizeof(float));

	if(amp_mod_buff == NULL){
		amp_mod_buff = new float[nb_sample];
	}

	it = note_active.begin();
	while( it != note_active.end()){

		release_mod(&(*it), amp_mod_buff, 0.2*srate, nb_sample, &end);

		octave = it->key / 12;
		note_val = it->key - octave * 12;
		shift = pow(2, octave);

		j = it->offset % wave_table_size[note_val];
		//j = it->offset;

		for(i = it->start_offset; i < end; i++){

			out[i] += wave_table[note_val][j] * amp_mod_buff[i] * amp;
			j += shift;
			if( j >= wave_table_size[note_val]){
				j = 0;
			}	
		}	
		it->offset = j;

		it->start_offset = 0;
		if(it->toDelete){
			it = note_active.erase(it);
		}else{
			it++;
		}
	}
}


void wave_gen::change_signal(uint32_t x, float y){
	if( x < wave_table_size[0]){
		wave_table[0][x] = y;
	}
	recalculate_table();
}

void wave_gen::change_signal(coord* pos, int nb){
	for(int i = 0; i < nb; i++){
		if( pos[i].x < wave_table_size[0]){
			wave_table[0][pos[i].x] = pos[i].y;
		}
	}
	recalculate_table();
}

void wave_gen::change_signal(float* val, int nb){

	transpose_graph(val, nb, wave_table[0], wave_table_size[0]);
	recalculate_table();

}

void wave_gen::recalculate_table(void){

	int error;
//	SRC_STATE* state = src_new(SRC_SINC_FASTEST , 1, &error);

//	if(error == 0){}


	for(int i = 1; i < 12; i++){
		src_conf.data_out = wave_table[i];
		src_conf.output_frames = wave_table_size[i];
		src_conf.src_ratio = freq_table[0]/freq_table[i];
	
		//error = src_process(state, &src_conf);	
		error = src_simple(&src_conf, SRC_SINC_FASTEST, 1);
		cout << "src param" << i << " ratio " << src_conf.src_ratio << " valin " << wave_table_size[i] << " valout " << src_conf.output_frames_gen << " error " << error << endl;
		
		wave_table_size[i] = src_conf.output_frames_gen;
	}	
}

void wave_gen::set_vol(float* vol_t){
	if(vol_t != NULL){
		if(amp != *vol_t){
			amp = *vol_t;
		}
	}
}
