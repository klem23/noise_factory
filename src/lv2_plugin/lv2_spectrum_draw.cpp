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
#include <lv2.h>

#include "spectrum_gen.hpp"
#include "spectrum_fft.hpp"

#ifdef OSC
 #include <lo/lo.h>
 #include <pthread.h>
#endif


#define SP_URI	"http://github.com/klem23/noise_factory/spectrum_draw";

using namespace std;

typedef struct{
	spectrum_gen* gen;
	spectrum_fft_out* fft;
	double* buff;
}spectrum_handle;

LV2_Descriptor *spectreDesc = NULL;
float* vol;

#ifdef OSC
lo_server los;

void lo_error(int num, const char *msg, const char *path){
        cout << " liblo server error " << num << " in path " << path << " : " << msg;
}

int signal_change_handler(const char *path, const char *types, lo_arg **argv, int argc,
                          void *data, void *user_data){
        cout << "OSC message receive " << argv[0]->i << argv[0]->f;
        spectrum_gen* sg = (spectrum_gen*)user_data;
        float* vec = (float*)lo_blob_dataptr(argv[0]);
        int size = lo_blob_datasize(argv[0]) / 4;
        sg->change_signal(vec, size);
/*
        printf("\nengine\n");
        for(int i = 0; i < 10; i++){
                printf(" %f ", vec[i]);
        }
        printf("\n");
*/

       return 1;
}

int vol_change_handler(const char *path, const char *types, lo_arg **argv, int argc,
						void *data, void *user_data){
	if(vol != NULL){
		*vol = argv[0]->f;
	}

	return 1;
}
#endif


void cleanup(LV2_Handle instance){
	spectrum_handle* hdl = (spectrum_handle*)instance;

	delete hdl->gen;
	delete hdl->fft;

	if( hdl->buff != NULL){
		fftw_free(hdl->buff);
	}
	if( hdl != NULL){
		free(hdl);
	}

	delete hdl;

	if( spectreDesc != NULL){
//		free(spectreDesc);
	}
}


void connectPort(LV2_Handle instance, uint32_t port, void* data){
	spectrum_handle* hdl = (spectrum_handle*)instance;

	switch(port){
		case 0:
			hdl->gen->set_input((float*)data);
			break;
		case 1:
			hdl->fft->set_output((float*)data);
			break;
		case 2:
			vol = (float*)data;
			break;
	}

}


LV2_Handle instantiate(const LV2_Descriptor *descriptor, 
			double s_rate, const char* path, 
			const LV2_Feature * const * features)
{

	spectrum_handle* hdl = new spectrum_handle;
	spectrum_gen* spectre = new spectrum_gen(s_rate);
	uint32_t table_size = spectre->get_table_size();

	hdl->buff = (double*)fftw_malloc(table_size * sizeof(double));
	memset(hdl->buff, 0, table_size*sizeof(double));
	spectre->set_output(hdl->buff);
	spectrum_fft_out* spectre_fft = new spectrum_fft_out(hdl->buff, table_size);
	hdl->gen = spectre;
	hdl->fft = spectre_fft;

#ifdef OSC
        /*liblo control*/
/*        los = lo_server_new("2323", lo_error);  
        //lo_server_add_method(los, NULL, NULL, generic_handler, NULL);
        lo_server_add_method(los,"/wave/point", "if", signal_change_handler, wave);
*/

        lo_server_thread lost = lo_server_thread_new("2324", NULL);
        lo_server_thread_add_method(lost, "/wave/table", "b", signal_change_handler, spectre);
        lo_server_thread_add_method(lost, "/wave/volume", "f", vol_change_handler, spectre);

        lo_server_thread_start(lost);
#endif



        for (int i = 0; features[i]; ++i) {
        	if (!strcmp(features[i]->URI, LV2_URID__map)) {
        		LV2_URID_Map* map = (LV2_URID_Map*)features[i]->data;
        		hdl->gen->set_midi_uri(
        				map->map(map->handle, LV2_MIDI__MidiEvent));
        		break;
        	}
        }

        return (LV2_Handle)hdl;
}


void run(LV2_Handle instance, uint32_t sample_count){
	spectrum_handle* hdl = (spectrum_handle*)instance;

	//clean buffer for connecting the two module
	uint32_t table_size = hdl->gen->get_table_size();
	memset(hdl->buff, 0, table_size*sizeof(double));

	hdl->gen->set_vol(vol);
	hdl->gen->process(sample_count);
	hdl->fft->process(sample_count);
}


void init(void){
	spectreDesc = (LV2_Descriptor*)malloc(sizeof(LV2_Descriptor));

	spectreDesc->URI = SP_URI;
	spectreDesc->activate = NULL;
	spectreDesc->deactivate = NULL;
	spectreDesc->connect_port = connectPort;
	spectreDesc->instantiate = instantiate;
	spectreDesc->cleanup = cleanup;
	spectreDesc->run = run;
	spectreDesc->extension_data = NULL;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index){
	if(!spectreDesc) init();

	switch(index){
	case 0:
		return spectreDesc;
	default:
		return NULL;
	}

}
