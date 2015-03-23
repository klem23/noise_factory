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


#include <samplerate.h>
#include <cmath>
#include <iostream>

#include "graph_tool.hpp"

void transpose_graph(float* graph, uint32_t in_size, float* table, uint32_t out_size){

        /*preparing samplerate for swapping from gui graph to plugin table*/
        SRC_DATA ref_src_conf;

        ref_src_conf.data_in = graph;
        ref_src_conf.input_frames = in_size;

        ref_src_conf.data_out = table;
        ref_src_conf.output_frames = out_size;
        ref_src_conf.src_ratio = (float)out_size/(float)in_size;


        int error = src_simple(&ref_src_conf, SRC_SINC_FASTEST, 1);
        std::cout << "REF src ratio " << ref_src_conf.src_ratio << " valin " << out_size << " valout " << ref_src_conf.output_frames_gen << " error " << error << std::endl;

}




void transpose_graph_log_to_linear(float* graph, uint32_t in_size, float* table, uint32_t out_size){

	/*out_size -> abs max for log graph*/
	//float max_log_scale_fsize = log10(out_size - 1);
	float max_log_scale_fsize = log(out_size - 1);
	
	uint32_t new_index[in_size];
	float log_idx;
	uint32_t new_idx;

	/*calcul new index of log scale point*/
	for(uint32_t i = 0; i < in_size; i++){
		/*log abs of point*/
		log_idx = (float)i * max_log_scale_fsize/(float)(in_size - 1);
		/*log to linear transform*/
		//new_idx = (uint32_t)pow(10.0, log_idx);
		new_idx = (uint32_t)exp(log_idx);
		if(new_idx >= out_size)
			new_idx = out_size - 1;
		new_index[i] = new_idx;

	} 
	
	/*interpolate graph*/
	table[0] = graph[0];
	for(uint32_t i = 1; i < in_size; i++){
		table[new_index[i]] = graph[i];
		uint32_t nb_pt = new_index[i] - new_index[i - 1];
		/*basic line equation*/
		float coeff = (table[new_index[i]] - table[new_index[i - 1]]) /(float)(new_index[i] - new_index[i - 1]);
		float b_origin = table[new_index[i]] - coeff * new_index[i];
		/*calcul missing point*/
		for(uint32_t j = 1; j < nb_pt; j++){
			table[new_index[i - 1] + j] =  coeff * (float)(new_index[i - 1] + j) + b_origin;
		}
	}
}

