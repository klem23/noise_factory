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

#include <cstdio> 
#include <cstdlib> 
#include <cstring> 
#include <lv2.h>

#ifdef OSC
 #include <lo/lo.h>
 #include <pthread.h>
#endif

#include "wave_gen.hpp"

#define WA_URI	"http://github.com/klem23/noise_factory/wave_draw";

using namespace std;

LV2_Descriptor *wavDesc = NULL;

float* table;
float* vol;

#ifdef OSC
lo_server los;
pthread_mutex_t wmutex;

void lo_error(int num, const char *msg, const char *path){
        cout << "liblo server error " << num << " in path " << path << ":" << msg << endl;
}

int signal_change_handler(const char *path, const char *types, lo_arg **argv, int argc,
		void *data, void *user_data){

	//pthread_mutex_lock(&wmutex);
	cout << "OSC message receive " << argv[0]->i << " : "<< argv[0]->f << endl;
	wave_gen* wg = (wave_gen*)user_data;
	float* vec = (float*)lo_blob_dataptr(argv[0]);
	uint32_t size = lo_blob_datasize(argv[0]) / sizeof(float);
	wg->change_signal(vec, size);
	//pthread_mutex_unlock(&wmutex);

	printf("\n engine  size %d \n", size);
	for(int i = 0; i < 10; i++){
		printf(" %f ", vec[i]);
		printf(" %f ", vec[size-i-1]);
	}
	printf("\n");

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
	wave_gen* wav = (wave_gen*)instance;
	delete wav;
	delete wavDesc;

#ifdef OSC
	pthread_mutex_destroy(&wmutex);
#endif

}


void connectPort(LV2_Handle instance, uint32_t port, void* data){
	wave_gen* plugin = (wave_gen*)instance;

	switch(port){
		case 0:
			plugin->set_input((float*)data);
			break;
		case 1:
			plugin->set_output((float*)data);
			break;
		case 2:
			table = (float*) data;
			break;
		case 3:
			vol = (float*) data;
			break;
	}

}


LV2_Handle instantiate(const LV2_Descriptor *descriptor, 
			double s_rate, const char* path, 
			const LV2_Feature * const * features)
{
	wave_gen* wave = new wave_gen(s_rate);

#ifdef OSC
	/*liblo control*/
/*        los = lo_server_new("2323", lo_error);  
        //lo_server_add_method(los, NULL, NULL, generic_handler, NULL);
        lo_server_add_method(los,"/wave/point", "if", signal_change_handler, wave);
*/

		pthread_mutex_init(&wmutex, NULL);
        lo_server_thread lost = lo_server_thread_new("2323", NULL);
        lo_server_thread_add_method(lost, "/wave/table", "b", signal_change_handler, wave);
        lo_server_thread_add_method(lost, "/wave/volume", "f", vol_change_handler, wave);

        lo_server_thread_start(lost);
#endif

        for (int i = 0; features[i]; ++i) {
                if (!strcmp(features[i]->URI, LV2_URID__map)) {
                        LV2_URID_Map* map = (LV2_URID_Map*)features[i]->data;
                        wave->set_midi_uri(
                                map->map(map->handle, LV2_MIDI__MidiEvent));
                        break;
                }
        }

	return (LV2_Handle)wave;
}


void run(LV2_Handle instance, uint32_t sample_count){

	//	if(pthread_mutex_trylock(&wmutex) == 0){

	//		printf("\nrun table ");
	//		for(int i = 0; i < 10 ; i++){
	//			printf("%f ", table[i]);
	//		}


	wave_gen* plugin = (wave_gen*)instance;

	/*liblo check before running*/
	//      lo_server_recv_noblock(los, 0);

	plugin->set_vol(vol);
	plugin->process(sample_count);
	//	}
}

const void *wave_extension(const char *uri){
	return NULL;
}

void init(void){
	wavDesc = new LV2_Descriptor;

	wavDesc->URI = WA_URI;
	wavDesc->activate = NULL;
	wavDesc->deactivate = NULL;
	wavDesc->connect_port = connectPort;
	wavDesc->instantiate = instantiate;
	wavDesc->cleanup = cleanup;
	wavDesc->run = run;
	//wavDesc->extension_data = NULL;
	wavDesc->extension_data = wave_extension;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index){
	if(!wavDesc) init();

	switch(index){
	case 0:
		return wavDesc;
	default:
		return NULL;
	}

}
