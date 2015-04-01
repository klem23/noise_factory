/*
	This file is part of the Noise Factory
	Copyright (C) 2009 Clément Guedez <klem.dev@gmail.com>


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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include "lv2.h"

#include "lv2/lv2plug.in/ns/ext/atom/util.h"
#include "lv2/lv2plug.in/ns/ext/urid/urid.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"

#include "beat_factory.h"

#define HE_URI       "http://github.com/klem23/noise_factory/beat_factory";
#define HE_MIDI      0
#define HE_OUTPUT    1

LV2_URID midi_Event;

LV2_Descriptor *bfDesc = NULL;

void cleanupBF(LV2_Handle instance)
{
	BF *plugin = (BF*)instance;

	if(plugin->temp_buff != NULL){
		free(plugin->temp_buff);
	}
	if(plugin->osc1 != NULL){
		free(plugin->osc1);
	}
	if( plugin != NULL){
		free(plugin);
	}/*
	if( bfDesc != NULL){
		free(bfDesc);
	}*/
}

void connectPortBF(LV2_Handle instance, uint32_t port,
			   void *data)
{
	BF *plugin = (BF*)instance;

	switch (port) {
	case HE_MIDI:
		plugin->midi_in = data;
		break;
	case HE_OUTPUT:
		plugin->audio_out = data;
		break;
	case 2:
		plugin->osc1->lv2_type = data;
		break;
	case 3:
		plugin->osc1->freq_start = data;
		break;
	case 4:
		plugin->osc1->freq_end = data;
		break;
	case 5:
		plugin->osc1->amp_env = data;
		break;
	case 6:
		plugin->osc1->freq_env = data;
		break;
        case 7:
		plugin->osc1->vol = data;
		break;
        case 8:
                plugin->osc2_active = data;
                break;
        case 9:
                plugin->osc2->lv2_type = data;
                break;
        case 10:
                plugin->osc2->freq_start = data;
                break;
        case 11:
                plugin->osc2->freq_end = data;
                break;
        case 12:
                plugin->osc2->amp_env = data;
                break;
        case 13:
                plugin->osc2->freq_env = data;
                break;
        case 14:
                plugin->osc2->vol = data;
                break;
	}
}

LV2_Handle instantiateBF(const LV2_Descriptor *descriptor,
	    double s_rate, const char *path,
	    const LV2_Feature * const* features)
{

	BF* plugin_data = (BF*)malloc(sizeof(BF));
	memset(plugin_data, 0, sizeof(BF));

	osc* osci = (osc*)malloc(2*sizeof(osc));
	memset(osci, 0, 2*sizeof(osc));

	plugin_data->sample_rate = s_rate;
	plugin_data->active = 0;
	plugin_data->osc1 = &(osci[0]);
	plugin_data->osc1->type = 0;
	plugin_data->osc1->lv2_type = NULL;
	plugin_data->osc2 = &(osci[1]);
	plugin_data->osc2_active = false;
	plugin_data->osc2->type = 0;
	plugin_data->temp_buff = NULL;

	int i;
        for(i = 0; features[i]; ++i) {
                if (!strcmp(features[i]->URI, LV2_URID__map)) {
                        LV2_URID_Map* map = (LV2_URID_Map*)features[i]->data;
                        midi_Event = map->map(map->handle, LV2_MIDI__MidiEvent);
                        break;
                }
        }



	return (LV2_Handle)plugin_data;
}

void runBF(LV2_Handle instance, uint32_t sample_count){

	//uint8_t midi_type, *events,
	uint8_t *data;
	//uint32_t event_count = 0;
	//LV2_Event* event;
	int i;
	bool end, release;

	BF* plugin = (BF*)instance;
	float * const output = plugin->audio_out;
	//LV2_Event_Buffer* midi_in = (LV2_Event_Buffer*)plugin->midi_in;

	if(plugin->temp_buff == NULL){
		plugin->temp_buff = (float*)malloc(sample_count * sizeof(float));
	}
	memset(plugin->temp_buff, 0, sample_count * sizeof(float));
	memset(output, 0, sample_count * sizeof(float));

	release = false;
	/*midi input parsing*/
	/*
	events = (uint8_t*) midi_in->data;
	while(midi_in->event_count > event_count){
		event = (LV2_Event*) events;
		data = events + sizeof(LV2_Event);
		midi_type = data[0];
		if((midi_type & 0xF0) == 0x90){
			if( plugin->active == 1 ){
				release = true;
			}else{
				plugin->active = 1;
				plugin->osc1->osc_inc = 0;
				plugin->osc2->osc_inc = 0;
			}
		}
		event_count++;
		/*moving pointer to next event with 64 bit padding*//*
		events += (sizeof(LV2_Event) + event->size + 7) & (~7);
	}*/

	/*midi input parsing v2*/
	LV2_Atom_Sequence* midi_in = (LV2_Atom_Sequence*)plugin->midi_in;	
	LV2_ATOM_SEQUENCE_FOREACH(midi_in, ev){
                if(ev->body.type == midi_Event){
			data = (const uint8_t*)LV2_ATOM_BODY(&ev->body);
			if((data[0] & 0xF0) == 0x90){
			        if( plugin->active == 1 ){
                        	        release = true;
                        	}else{
                                	plugin->active = 1;
                                	plugin->osc1->osc_inc = 0;
                                	plugin->osc2->osc_inc = 0;
                        	}	
                	}
		}
	}

	/*audio ouput writing*/
	if(plugin->active == 1){
		freq_env(plugin->osc1, sample_count, plugin->sample_rate, plugin->temp_buff);
	
		/*type support by potentiometer*/
		//if(plugin->osc1->lv2_type != NULL){
		//	plugin->osc1->type = (int)*plugin->osc1->lv2_type;
		//}
	
		switch((int)*plugin->osc1->lv2_type){
		    case 0:
			osc_sin(plugin->osc1, sample_count, plugin->sample_rate, plugin->temp_buff, output);
			break;
		    case 1:
			osc_square(plugin->osc1, sample_count, plugin->sample_rate, plugin->temp_buff, output);
			break;
		    case 2:
			osc_saw_up(plugin->osc1, sample_count, plugin->sample_rate, plugin->temp_buff, output);
			break;
		    case 3:
			osc_saw_down(plugin->osc1, sample_count, plugin->sample_rate, plugin->temp_buff, output);
			break;
		    case 4:
			osc_triangle(plugin->osc1, sample_count, plugin->sample_rate, plugin->temp_buff, output);
			break;
		}
		amp_env(plugin->osc1, sample_count, plugin->sample_rate, output);

		/*OSC 2*/
		if((int)(*plugin->osc2_active) != 0){
			freq_env(plugin->osc2, sample_count, plugin->sample_rate, plugin->temp_buff);
			switch((int)*plugin->osc2->lv2_type){
		    		case 0:
				      osc_sin(plugin->osc2, sample_count, plugin->sample_rate, plugin->temp_buff, plugin->temp_buff);
				      break;
		    		case 1:
				      osc_square(plugin->osc2, sample_count, plugin->sample_rate, plugin->temp_buff, plugin->temp_buff);
				      break;
		    		case 2:
				      osc_saw_up(plugin->osc2, sample_count, plugin->sample_rate, plugin->temp_buff, plugin->temp_buff);
				      break;
		    		case 3:
				      osc_saw_down(plugin->osc2, sample_count, plugin->sample_rate, plugin->temp_buff, plugin->temp_buff);
				      break;
		    		case 4:
				      osc_triangle(plugin->osc2, sample_count, plugin->sample_rate, plugin->temp_buff, plugin->temp_buff);
				      break;
			}
			amp_env(plugin->osc2, sample_count, plugin->sample_rate, plugin->temp_buff);

			for(i = 0; i < sample_count; i++){
				output[i] += plugin->temp_buff[i];
			}
		}

		end = true;
		for(i = 0; i < 10; i++){
			if(output[sample_count - i] != 0.0){
				end = false;
			}
		}
		if(end){
			plugin->active = 0;
		}
		
		if(release){
			release_prev(output, sample_count, plugin->sample_rate); 
			plugin->active = 1;
	                plugin->osc1->osc_inc = 0;
                	plugin->osc2->osc_inc = 0;
		}
	
	}
}

void init()
{
	bfDesc =
	 (LV2_Descriptor *)malloc(sizeof(LV2_Descriptor));

	bfDesc->URI = HE_URI;
	bfDesc->activate = NULL;
	bfDesc->cleanup = cleanupBF;
	bfDesc->connect_port = connectPortBF;
	bfDesc->deactivate = NULL;
	bfDesc->instantiate = instantiateBF;
	bfDesc->run = runBF;
	//bfDesc->extension_data = bf_ext;
	bfDesc->extension_data = NULL;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor *lv2_descriptor(uint32_t index)
{
	if (!bfDesc) init();


	switch (index) {
	case 0:
		return bfDesc;
	default:
		return NULL;
	}

}
