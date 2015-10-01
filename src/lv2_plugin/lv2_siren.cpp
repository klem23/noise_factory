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
		/*OSC*/
		case 2:
			op_t.type = (float*)data;
			break;
		case 3:
			op_t.freq = (float*)data;
			break;
		case 4:
			op_t.phase = (float*)data;
			break;
		case 5:
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
                        break;
                case 28:
                        op_t.pe.ae = (float*)data;
                        break;
	}

}


LV2_Handle instantiate(const LV2_Descriptor *descriptor, 
		double s_rate, const char* path,
		const LV2_Feature * const * features)
{
	siren* sir = new siren(s_rate);

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
