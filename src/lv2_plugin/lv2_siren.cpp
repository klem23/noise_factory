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
#include <cstring> 
#include <lv2.h>

#include "siren.hpp"

#define SIREN_URI	"http://github.com/klem23/noise_factory/siren";

using namespace std;

LV2_Descriptor *siDesc = NULL;
si_param sp_t;
osc_param op_t;

void cleanup(LV2_Handle instance){
	siren* si = (siren*)instance;
	delete si;

	delete siDesc;

}


void connectPort(LV2_Handle instance, uint32_t port, void* data){
	siren* plugin = (siren*)instance;

	switch(port){
		/*basic I/O*/
		case 0:
			plugin->set_input((float*)data);
			break;
		case 1:
			plugin->set_output((float*)data);
			break;
		/*control param*/
		case 2:
			sp_t.kmod = (float*)data;
			break;
		case 3:
			sp_t.kpitch = (float*)data;
			break;
		case 4:
			op_t.type = (float*)data; /* OSC */
			break;
		case 5:
			sp_t.pitch = (float*)data;
			break;
		case 6:
			op_t.fl.type = (float*)data; /* OSC*/
			break;
		case 7:
			op_t.fl.freq = (float*)data; /* OSC */
			break;
		case 8:
			op_t.fl.amt = (float*)data; /* OSC */
			break;
		case 9:
			op_t.ae.RR = (float*)data; /* OSC */
			break;
		case 10:
			op_t.amp = (float*)data; /* OSC */
			break;

	}

}


LV2_Handle instantiate(const LV2_Descriptor *descriptor, 
		double s_rate, const char* path,
		const LV2_Feature * const * features)
{
	siren* sir = new siren(s_rate);

	memset(&sp_t, 0, sizeof(si_param));
	memset(&op_t, 0, sizeof(osc_param));


	op_t.freq = new float;
	*op_t.freq = 0;
	op_t.phase = new float;
	*op_t.phase = 0;

	op_t.fl.active = new float;

	op_t.ae.active = new float;
	op_t.ae.AA = new float;
	*op_t.ae.AA = 0;
	op_t.ae.DD = new float;
	*op_t.ae.DD = 0;
	op_t.ae.SS = new float;
	*op_t.ae.SS = 1.0;
	op_t.ae.amt = new float;
	*op_t.ae.amt = 0;




	for (int i = 0; features[i]; ++i) {
		if (!strcmp(features[i]->URI, LV2_URID__map)) {
			LV2_URID_Map* map = (LV2_URID_Map*)features[i]->data;
			sir->set_midi_uri(
					map->map(map->handle, LV2_MIDI__MidiEvent));
			break;
		}
	}


	return (LV2_Handle)sir;
}


void run(LV2_Handle instance, uint32_t sample_count){
	siren* plugin = (siren*)instance;
	plugin->si_check_param(&sp_t, &op_t);
	plugin->process(sample_count);
}


void init(void){
	siDesc = new LV2_Descriptor;

	siDesc->URI = SIREN_URI;
	siDesc->activate = NULL;
	siDesc->deactivate = NULL;
	siDesc->connect_port = connectPort;
	siDesc->instantiate = instantiate;
	siDesc->cleanup = cleanup;
	siDesc->run = run;
	siDesc->extension_data = NULL;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index){
	if(!siDesc) init();

	switch(index){
	case 0:
		return siDesc;
	default:
		return NULL;
	}

}
