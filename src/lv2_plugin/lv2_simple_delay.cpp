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

#include "simple_delay.hpp"

#define SD_URI	"http://lv2plug.in/plugins/noise_factory/simple_delay";

using namespace std;

LV2_Descriptor *sdDesc = NULL;
simple_delay_param sdp;

void cleanup(LV2_Handle instance){
	simple_delay* sd = (simple_delay*)instance;
	delete sd;

	if( sdDesc != NULL){
		//free(oscDesc);
	}
}


void connectPort(LV2_Handle instance, uint32_t port, void* data){
	simple_delay* plugin = (simple_delay*)instance;

	switch(port){
		case 0:
			plugin->set_input((float*)data);
			break;
		case 1:
			plugin->set_output((float*)data);
			break;
		case 2:
			sdp.nbe = (float*)data;
			break;
		case 3:
			sdp.ds = (float*)data;
			break;
		case 4:
			sdp.dms = (float*)data;
			break;
		case 5:
			sdp.fb = (float*)data;
			break;
		case 6:
			sdp.cs = (float*)data;
			break;
	}

}


LV2_Handle instantiate(const LV2_Descriptor *descriptor, 
			double s_rate, const char* path, 
			const LV2_Feature * const * features)
{
	simple_delay* sd = new simple_delay(s_rate);

	return (LV2_Handle)sd;
}


void run(LV2_Handle instance, uint32_t sample_count){
	simple_delay* plugin = (simple_delay*)instance;
	plugin->check_param(&sdp);
	plugin->process(sample_count);
}


void init(void){
	sdDesc = (LV2_Descriptor*)malloc(sizeof(LV2_Descriptor));

	sdDesc->URI = SD_URI;
	sdDesc->activate = NULL;
	sdDesc->deactivate = NULL;
	sdDesc->connect_port = connectPort;
	sdDesc->instantiate = instantiate;
	sdDesc->cleanup = cleanup;
	sdDesc->run = run;
	sdDesc->extension_data = NULL;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index){
	if(!sdDesc) init();

	switch(index){
	case 0:
		return sdDesc;
	default:
		return NULL;
	}

}
