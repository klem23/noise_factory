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

#include "osc.hpp"

#define BF_URI	"http://github.com/klem23/noise_factory/beat_factory_tuned";

using namespace std;

LV2_Descriptor *bfDesc = NULL;
bf_param bf_t;

void cleanup(LV2_Handle instance){
	osc* osci = (osc*)instance;
	delete osci;

	if( bfDesc != NULL){
		//free(bfDesc);
	}
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
	}

}


LV2_Handle instantiate(const LV2_Descriptor *descriptor, 
			double s_rate, const char* path, 
			const LV2_Feature * const * features)
{
	osc* osci = new osc(s_rate);

	return (LV2_Handle)osci;
}


void run(LV2_Handle instance, uint32_t sample_count){
	osc* plugin = (osc*)instance;
	plugin->check_param(&op_t);
	plugin->process(sample_count);
}


void init(void){
	bfDesc = (LV2_Descriptor*)malloc(sizeof(LV2_Descriptor));

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
