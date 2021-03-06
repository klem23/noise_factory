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

#include "osc.hpp"

#define BF_URI	"http://github.com/klem23/noise_factory/beat_factory_tuned";

using namespace std;

LV2_Descriptor *bfDesc = NULL;
osc_param op_t;

void cleanup(LV2_Handle instance){
	osc* osci = (osc*)instance;
	delete osci;

	delete bfDesc;

}


void connectPort(LV2_Handle instance, uint32_t port, void* data){
	osc* plugin = (osc*)instance;

	switch(port){
		/*basic I/O*/
		case 0:
			plugin->set_input((float*)data);
			break;
		case 1:
			plugin->set_output((float*)data);
			break;
		/*OSC*/
		case 2:
			//plugin->set_type((float*)data);
			op_t.type = (float*)data;
			break;
		case 3:
			//plugin->set_freq((float*)data);
			op_t.freq = (float*)data;
			break;
		case 4:
			//plugin->set_phase((float*)data);
			op_t.phase = (float*)data;
			break;
		case 5:
			//plugin->set_amp((float*)data);
			op_t.amp = (float*)data;
			break;
		/*freq env*/
		case 6:
			op_t.fe.active = (float*)data;
			break;
		case 7:
			op_t.fe.AA = (float*)data;
			break;
		case 8:
			op_t.fe.DD = (float*)data;
			break;
		case 9:
			op_t.fe.SS = (float*)data;
			break;
		case 10:
			op_t.fe.RR = (float*)data;
			break;
		/*freq lfo*/
		case 11:
			op_t.fl.active = (float*)data;
			break;
		case 12:
			op_t.fl.type = (float*)data;
			break;
		case 13:
			op_t.fl.freq = (float*)data;
			break;
		case 14:
			op_t.fl.amt = (float*)data;
			break;
		/*amp env*/
		case 15:
			op_t.ae.active = (float*)data;
			break;
		case 16:
			op_t.ae.AA = (float*)data;
			break;
		case 17:
			op_t.ae.DD = (float*)data;
			break;
		case 18:
			op_t.ae.SS = (float*)data;
			break;
		case 19:
			op_t.ae.RR = (float*)data;
			break;
		/*amp lfo*/
		case 20:
			op_t.al.active = (float*)data;
			break;
		case 21:
			op_t.al.type = (float*)data;
			break;
		case 22:
			op_t.al.freq = (float*)data;
			break;
		case 23:
			op_t.al.amt = (float*)data;
			break;
		case 24:
			op_t.pe.factive = (float*)data;
			break;
		case 25:
			op_t.pe.fe = (float*)data;
			break;
		case 26:
			op_t.pe.fs = (float*)data;
			break;
		case 27:
			op_t.pe.aactive = (float*)data;
		case 28:
			op_t.pe.ae = (float*)data;
			break;
	}

}


LV2_Handle instantiate(const LV2_Descriptor *descriptor, 
			double s_rate, const char* path, 
			const LV2_Feature * const * features)
{
	osc* osci = new osc(s_rate);

	memset(&op_t, 0, sizeof(osc_param));

        for (int i = 0; features[i]; ++i) {
        	if (!strcmp(features[i]->URI, LV2_URID__map)) {
                	LV2_URID_Map* map = (LV2_URID_Map*)features[i]->data;
                        osci->set_midi_uri(
                                        map->map(map->handle, LV2_MIDI__MidiEvent));
                        break;
                }
        }




	return (LV2_Handle)osci;
}


void run(LV2_Handle instance, uint32_t sample_count){
	osc* plugin = (osc*)instance;
	plugin->check_param(&op_t);
	plugin->process(sample_count);
}


void init(void){
	bfDesc = new LV2_Descriptor;

	bfDesc->URI = BF_URI;
	bfDesc->activate = NULL;
	bfDesc->deactivate = NULL;
	bfDesc->connect_port = connectPort;
	bfDesc->instantiate = instantiate;
	bfDesc->cleanup = cleanup;
	bfDesc->run = run;
	bfDesc->extension_data = NULL;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index){
	if(!bfDesc) init();

	switch(index){
	case 0:
		return bfDesc;
	default:
		return NULL;
	}

}
