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
#include <iostream> 
#include <lv2.h>

#include "conv_filter.hpp"

#ifdef OSC
 #include <lo/lo.h>
 #include <pthread.h>
#endif


#define ZP_URI	"http://lv2plug.in/plugins/noise_factory/conv_filter";

using namespace std;

LV2_Descriptor *cfDesc = NULL;
//filter_param fp;



#ifdef OSC
lo_server los;

void lo_error(int num, const char *msg, const char *path){
        cout << " liblo server error " << num << "in path " << path << msg << endl;
}

int signal_change_handler(const char *path, const char *types, lo_arg **argv, int argc,
                          void *data, void *user_data){
        cout << "OSC message receive " << argv[0]->i << argv[0]->f << endl;
        conv_filter* cf = (conv_filter*)user_data;
        float* vec = (float*)lo_blob_dataptr(argv[0]);
        int size = lo_blob_datasize(argv[0]) / 4;
        cf->set_spectrum(vec, size);

       return 1;
}

#endif




void cleanup(LV2_Handle instance){
	conv_filter* cf = (conv_filter*)instance;
	delete cf;

	if( cfDesc != NULL){
		//free(zpDesc);
	}
}


void connectPort(LV2_Handle instance, uint32_t port, void* data){
	conv_filter* plugin = (conv_filter*)instance;

	switch(port){
		case 0:
			plugin->set_input((float*)data);
			break;
		case 1:
			plugin->set_output((float*)data);
			break;
		case 2:
			plugin->set_vol((float*)data);
			break;
	}

}


LV2_Handle instantiate(const LV2_Descriptor *descriptor, 
			double s_rate, const char* path, 
			const LV2_Feature * const * features)
{
	conv_filter* cf = new conv_filter(s_rate);


#ifdef OSC
        /*liblo control*/
        lo_server_thread lost = lo_server_thread_new("2326", NULL);
        lo_server_thread_add_method(lost, "/wave/table", "b", signal_change_handler, cf);

        lo_server_thread_start(lost);
#endif



	return (LV2_Handle)cf;
}


void run(LV2_Handle instance, uint32_t sample_count){
	conv_filter* plugin = (conv_filter*)instance;

//	plugin->check_param(&fp);
	plugin->process(sample_count);
}


void init(void){
	cfDesc = (LV2_Descriptor*)malloc(sizeof(LV2_Descriptor));

	cfDesc->URI = ZP_URI;
	cfDesc->activate = NULL;
	cfDesc->deactivate = NULL;
	cfDesc->connect_port = connectPort;
	cfDesc->instantiate = instantiate;
	cfDesc->cleanup = cleanup;
	cfDesc->run = run;
	cfDesc->extension_data = NULL;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index){
	if(!cfDesc) init();

	switch(index){
	case 0:
		return cfDesc;
	default:
		return NULL;
	}

}
