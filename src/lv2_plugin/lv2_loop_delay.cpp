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

#include "loop_delay.hpp"

#define LD_URI	"http://github.com/klem23/noise_factory/loop_delay";

using namespace std;

LV2_Descriptor *ldDesc = NULL;
loop_delay_param ldp;

void cleanup(LV2_Handle instance){
	loop_delay* ld = (loop_delay*)instance;
	delete ld;

	delete ldDesc;

}


void connectPort(LV2_Handle instance, uint32_t port, void* data){
	loop_delay* plugin = (loop_delay*)instance;

	switch(port){
		case 0:
			plugin->set_input((float*)data);
			break;
		case 1:
			plugin->set_output((float*)data);
			break;
		case 2:
			ldp.nbe = (float*)data;
			break;
		case 3:
			ldp.ds = (float*)data;
			break;
		case 4:
			ldp.dms = (float*)data;
			break;
		case 5:
			ldp.fb = (float*)data;
			break;
		case 6:
			ldp.cs = (float*)data;
			break;
	}

}


LV2_Handle instantiate(const LV2_Descriptor *descriptor, 
			double s_rate, const char* path, 
			const LV2_Feature * const * features)
{
	loop_delay* ld = new loop_delay(s_rate);

	return (LV2_Handle)ld;
}


void run(LV2_Handle instance, uint32_t sample_count){
	loop_delay* plugin = (loop_delay*)instance;
	plugin->check_param(&ldp);
	plugin->process(sample_count);
}


void init(void){
	ldDesc = new LV2_Descriptor;

	ldDesc->URI = LD_URI;
	ldDesc->activate = NULL;
	ldDesc->deactivate = NULL;
	ldDesc->connect_port = connectPort;
	ldDesc->instantiate = instantiate;
	ldDesc->cleanup = cleanup;
	ldDesc->run = run;
	ldDesc->extension_data = NULL;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index){
	if(!ldDesc) init();

	switch(index){
	case 0:
		return ldDesc;
	default:
		return NULL;
	}

}
