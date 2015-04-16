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

#include <cstdlib> 
#include <cstring> 
#include <iostream>
#include <lv2.h>

#include "pitch_shifter.hpp"
#include "spectrum_fft.hpp"

#define PS_URI	"http://github.com/klem23/noise_factory/pitch_shifter";

using namespace std;

typedef struct{
	pitch_shifter* ps;
	spectrum_fft_in* fft_in;
	spectrum_fft_out* fft_out;
	double* buff_in;
	double* buff_out;
}ps_handle;

ps_param psp;

LV2_Descriptor *psDesc = NULL;

void cleanup(LV2_Handle instance){
	ps_handle* hdl = (ps_handle*)instance;

	delete hdl->ps;
	delete hdl->fft_in;
	delete hdl->fft_out;


	if( hdl->buff_in != NULL){
		fftw_free(hdl->buff_in);
	}

	if( hdl->buff_out != NULL){
		fftw_free(hdl->buff_out);
	}

	delete hdl;

	if( psDesc != NULL){
		//		free(fftfDesc);
	}
}


void connectPort(LV2_Handle instance, uint32_t port, void* data){
	ps_handle* hdl = (ps_handle*)instance;

	switch(port){
		case 0:
			hdl->fft_in->set_input((float*)data);
			break;
		case 1:
			hdl->fft_out->set_output((float*)data);
			break;
		case 2:
			psp.freq_shift = (float*)data;
			break;
		case 3:
			psp.volume = (float*)data;
			break;
	}

}


LV2_Handle instantiate(const LV2_Descriptor *descriptor, 
			double s_rate, const char* path, 
			const LV2_Feature * const * features)
{

	ps_handle* hdl = new ps_handle;
	pitch_shifter* ps = new pitch_shifter(s_rate);
	uint32_t table_size = ps->get_table_size();

	hdl->buff_in = (double*)fftw_malloc(table_size * sizeof(double));
	hdl->buff_out = (double*)fftw_malloc(table_size * sizeof(double));
	memset(hdl->buff_in, 0, table_size*sizeof(double));
	memset(hdl->buff_out, 0, table_size*sizeof(double));
	ps->set_input(hdl->buff_in);
	ps->set_output(hdl->buff_out);

	spectrum_fft_in* spectre_fft_in = new spectrum_fft_in(hdl->buff_in, table_size);
	spectrum_fft_out* spectre_fft_out = new spectrum_fft_out(hdl->buff_out, table_size);
	hdl->ps = ps;
	hdl->fft_in = spectre_fft_in;
	hdl->fft_out = spectre_fft_out;

	return (LV2_Handle)hdl;
}


void run(LV2_Handle instance, uint32_t sample_count){
	ps_handle* hdl = (ps_handle*)instance;

	//clean buffer for connecting the two module
	uint32_t table_size = hdl->ps->get_table_size();
	memset(hdl->buff_in, 0, table_size*sizeof(double));
	memset(hdl->buff_out, 0, table_size*sizeof(double));

	hdl->fft_in->process(sample_count);
	hdl->ps->check_param(&psp);
	hdl->ps->process(sample_count);
	hdl->fft_out->process(sample_count);

}


void init(void){
	psDesc = (LV2_Descriptor*)malloc(sizeof(LV2_Descriptor));

	psDesc->URI = PS_URI;
	psDesc->activate = NULL;
	psDesc->deactivate = NULL;
	psDesc->connect_port = connectPort;
	psDesc->instantiate = instantiate;
	psDesc->cleanup = cleanup;
	psDesc->run = run;
	psDesc->extension_data = NULL;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index){
	if(!psDesc) init();

	switch(index){
	case 0:
		return psDesc;
	default:
		return NULL;
	}

}


/*
void sample_nb_run_change(int nb_sample){

	//spectrum gen
		spectrum_gen set size
			internal malloc
		spectrumm_fft set size
			fftw execute
		intermediate buffer


	//spectrum filter
		spectrum_filter set size
			internal malloc
		spectrum_fft set size * 2
			fftw execute
		intermediate buffer

	//conv filter



}

*/
