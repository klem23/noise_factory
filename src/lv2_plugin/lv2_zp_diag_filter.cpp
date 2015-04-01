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

#include "zp_diag_filter.hpp"

#define ZP_URI	"http://github.com/klem23/noise_factory/recursive_filter";

using namespace std;

LV2_Descriptor *zpDesc = NULL;
filter_param fp;

void cleanup(LV2_Handle instance){
	zp_diag_filter* zp = (zp_diag_filter*)instance;
	delete zp;

	if( zpDesc != NULL){
		//free(zpDesc);
	}
}


void connectPort(LV2_Handle instance, uint32_t port, void* data){
	zp_diag_filter* plugin = (zp_diag_filter*)instance;

	switch(port){
		case 0:
			plugin->set_input((float*)data);
			break;
		case 1:
			plugin->set_output((float*)data);
			break;
		case 2:
			fp.band_type = (float*)data;
			break;
		case 3:
			fp.filter_type = (float*)data;
			break;
		case 4:
			fp.order = (float*)data;
			break;
		case 5:
			fp.cutoff = (float*)data;
			break;
		case 6:
			fp.rp = (float*)data;
			break;
	}

}


LV2_Handle instantiate(const LV2_Descriptor *descriptor, 
			double s_rate, const char* path, 
			const LV2_Feature * const * features)
{
	zp_diag_filter* zp = new zp_diag_filter(s_rate, 0);

	return (LV2_Handle)zp;
}


void run(LV2_Handle instance, uint32_t sample_count){
	zp_diag_filter* plugin = (zp_diag_filter*)instance;

	plugin->check_param(&fp);
	plugin->process(sample_count);
}


void init(void){
	zpDesc = (LV2_Descriptor*)malloc(sizeof(LV2_Descriptor));

	zpDesc->URI = ZP_URI;
	zpDesc->activate = NULL;
	zpDesc->deactivate = NULL;
	zpDesc->connect_port = connectPort;
	zpDesc->instantiate = instantiate;
	zpDesc->cleanup = cleanup;
	zpDesc->run = run;
	zpDesc->extension_data = NULL;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index){
	if(!zpDesc) init();

	switch(index){
	case 0:
		return zpDesc;
	default:
		return NULL;
	}

}
