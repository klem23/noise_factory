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

#include "osc.hpp"
#include "zp_diag_filter.hpp"


#define CS_URI	"http://github.com/klem23/noise_factory/classic_synth";

using namespace std;

typedef struct{
	osc* osc1;
	osc* osc2;
	zp_diag_filter* filter;
	float* buff;
}cs_handle;

osc_param op1_t;
osc_param op2_t;
filter_param fp;

LV2_Descriptor *csDesc = NULL;

void cleanup(LV2_Handle instance){
	cs_handle* hdl = (cs_handle*)instance;

	delete hdl->osc1;
	delete hdl->osc2;
	delete hdl->filter;
	delete[] hdl->buff;

	delete hdl;

	delete csDesc;
}


void connectPort(LV2_Handle instance, uint32_t port, void* data){
	cs_handle* hdl = (cs_handle*)instance;

	switch(port){
		case 0:
			hdl->osc1->set_input((float*)data);
			hdl->osc2->set_input((float*)data);
			break;
		case 1:
			hdl->filter->set_output((float*)data);
			break;
	}

}


LV2_Handle instantiate(const LV2_Descriptor *descriptor, 
			double s_rate, const char* path, 
			const LV2_Feature * const * features)
{

	memset(&op1_t, 0, sizeof(osc_param));
	memset(&op2_t, 0, sizeof(osc_param));
	memset(&fp, 0, sizeof(filter_param));


	cs_handle* hdl = new cs_handle;
	hdl->osc1 = new osc(s_rate);
	hdl->osc2 = new osc(s_rate);
	hdl->filter = new zp_diag_filter(s_rate, 0);

	hdl->buff = NULL;

     	for (int i = 0; features[i]; ++i) {
                if (!strcmp(features[i]->URI, LV2_URID__map)) {
                        LV2_URID_Map* map = (LV2_URID_Map*)features[i]->data;
                        hdl->osc1->set_midi_uri(
                                map->map(map->handle, LV2_MIDI__MidiEvent));
                        hdl->osc2->set_midi_uri(
                                map->map(map->handle, LV2_MIDI__MidiEvent));
                        break;
                }
        }

	return (LV2_Handle)hdl;
}


void run(LV2_Handle instance, uint32_t nb_sample){
	cs_handle* hdl = (cs_handle*)instance;

	if(hdl->buff == NULL){
		hdl->buff = new float[nb_sample];
		hdl->osc1->set_output(hdl->buff);
		hdl->osc2->set_output(hdl->buff);
		hdl->filter->set_input(hdl->buff);
	}

	//clean buffer for connecting the two module
	memset(hdl->buff, 0, nb_sample*sizeof(float));

	hdl->osc1->check_param(&op1_t);
	hdl->osc2->check_param(&op2_t);
	hdl->filter->check_param(&fp);

	hdl->osc1->process(nb_sample);
	hdl->osc2->process(nb_sample);
	hdl->filter->process(nb_sample);
}


void init(void){
	csDesc = new LV2_Descriptor;

	csDesc->URI = CS_URI;
	csDesc->activate = NULL;
	csDesc->deactivate = NULL;
	csDesc->connect_port = connectPort;
	csDesc->instantiate = instantiate;
	csDesc->cleanup = cleanup;
	csDesc->run = run;
	csDesc->extension_data = NULL;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index){
	if(!csDesc) init();

	switch(index){
	case 0:
		return csDesc;
	default:
		return NULL;
	}

}
