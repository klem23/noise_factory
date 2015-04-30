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

#include "spectrum_filter.hpp"
#include "spectrum_fft.hpp"

#ifdef OSC
 #include <lo/lo.h>
 #include <pthread.h>
#endif


#define SPF_URI	"http://github.com/klem23/noise_factory/spectrum_filter";

using namespace std;

typedef struct{
	spectrum_filter* filter;
	spectrum_fft_in* fft_in;
	spectrum_fft_out* fft_out;
	double* buff_in;
	double* buff_out;
}spf_handle;

spf_param sp;

LV2_Descriptor *spfDesc = NULL;


#ifdef OSC
lo_server los;

void lo_error(int num, const char *msg, const char *path){
        printf("liblo server error %d in path %s: %s\n", num, path, msg);
}

int signal_change_handler(const char *path, const char *types, lo_arg **argv, int argc,
                          void *data, void *user_data){
        //printf("OSC message receive %d, %f\n", argv[0]->i, argv[0]->f);
        spectrum_filter* spf = (spectrum_filter*)user_data;
        float* vec = (float*)lo_blob_dataptr(argv[0]);
        uint32_t size = lo_blob_datasize(argv[0]) / 4;
        spf->set_spectrum(vec, size);
/*
        printf("\nengine\n");
        for(int i = 0; i < 10; i++){
                printf(" %f ", vec[i]);
        }
        printf("\n");
*/

       return 1;
}

int freq_change_handler(const char *path, const char *types, lo_arg **argv, int argc,
                          void *data, void *user_data){

	if(sp.freq_shift != NULL){
		*sp.freq_shift = argv[0]->f;
	}

	return 1;
}

int vol_change_handler(const char *path, const char *types, lo_arg **argv, int argc,
                          void *data, void *user_data){
	if(sp.volume != NULL){
			*sp.volume = argv[0]->f;
		}

	return 1;
}
#endif


void cleanup(LV2_Handle instance){
	spf_handle* hdl = (spf_handle*)instance;

	delete hdl->filter;
	delete hdl->fft_in;
	delete hdl->fft_out;


	if( hdl->buff_in != NULL){
		fftw_free(hdl->buff_in);
	}

	if( hdl->buff_out != NULL){
		fftw_free(hdl->buff_out);
	}

	delete hdl;
	delete spfDesc;
}


void connectPort(LV2_Handle instance, uint32_t port, void* data){
	spf_handle* hdl = (spf_handle*)instance;

	switch(port){
		case 0:
			hdl->fft_in->set_input((float*)data);
			break;
		case 1:
			hdl->fft_out->set_output((float*)data);
			break;
		case 2:
			sp.freq_shift = (float*)data;
			break;
		case 3:
			sp.volume = (float*)data;
			break;
	}

}


LV2_Handle instantiate(const LV2_Descriptor *descriptor, 
			double s_rate, const char* path, 
			const LV2_Feature * const * features)
{

	spf_handle* hdl = new spf_handle;
	spectrum_filter* filter = new spectrum_filter(s_rate);
	uint32_t table_size = filter->get_table_size();

	hdl->buff_in = (double*)fftw_malloc(table_size * sizeof(double));
	hdl->buff_out = (double*)fftw_malloc(table_size * sizeof(double));
	memset(hdl->buff_in, 0, table_size*sizeof(double));
	memset(hdl->buff_out, 0, table_size*sizeof(double));
	filter->set_input(hdl->buff_in);
	filter->set_output(hdl->buff_out);

	spectrum_fft_in* spectre_fft_in = new spectrum_fft_in(hdl->buff_in, table_size);
	spectrum_fft_out* spectre_fft_out = new spectrum_fft_out(hdl->buff_out, table_size);
	hdl->filter = filter;
	hdl->fft_in = spectre_fft_in;
	hdl->fft_out = spectre_fft_out;

#ifdef OSC
        /*liblo control*/
/*        los = lo_server_new("2323", lo_error);  
        //lo_server_add_method(los, NULL, NULL, generic_handler, NULL);
        lo_server_add_method(los,"/wave/point", "if", signal_change_handler, wave);
*/

        lo_server_thread lost = lo_server_thread_new("2325", NULL);
        lo_server_thread_add_method(lost, "/wave/table", "b", signal_change_handler, filter);
        lo_server_thread_add_method(lost, "/wave/freq_shift", "f", freq_change_handler, filter);
        lo_server_thread_add_method(lost, "/wave/volume", "f", vol_change_handler, filter);

        lo_server_thread_start(lost);
#endif


	return (LV2_Handle)hdl;
}


void run(LV2_Handle instance, uint32_t sample_count){
	spf_handle* hdl = (spf_handle*)instance;

	//clean buffer for connecting the two module
	uint32_t table_size = hdl->filter->get_table_size();
	memset(hdl->buff_in, 0, table_size*sizeof(double));
	memset(hdl->buff_out, 0, table_size*sizeof(double));

	hdl->fft_in->process(sample_count);
	hdl->filter->check_param(&sp);
	hdl->filter->process(sample_count);
	hdl->fft_out->process(sample_count);

}


void init(void){
	spfDesc = new LV2_Descriptor;

	spfDesc->URI = SPF_URI;
	spfDesc->activate = NULL;
	spfDesc->deactivate = NULL;
	spfDesc->connect_port = connectPort;
	spfDesc->instantiate = instantiate;
	spfDesc->cleanup = cleanup;
	spfDesc->run = run;
	spfDesc->extension_data = NULL;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index){
	if(!spfDesc) init();

	switch(index){
	case 0:
		return spfDesc;
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
