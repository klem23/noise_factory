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


#include <cmath>
#include <iostream>
#include <algorithm>

#include "midi_parser.hpp"
//#include "lv2_event.h"

#include "lv2/lv2plug.in/ns/ext/atom/util.h"

midi_parser::midi_parser(uint32_t sampling_rate)
	:srate(sampling_rate)
	,in(NULL),midi_Event(0){

	for(int i = 0; i < 127; i++){
		//freq_table[i] = 440.0 * pow(2.0, (i-69.0)/12.0) / srate; 
		freq_table[i] = 8.176 * pow(2.0, i/12.0) / srate; 
	}
}


void midi_parser::note_on(note* n_note){
	note_active.push_back(*n_note);
}

void midi_parser::note_off(note* n_note){
	if(!note_active.empty()){
		//it_del = search(note_active.begin(), note_active.end(), n_note, n_note, note::note_equal);
		it = find(note_active.begin(), note_active.end(), n_note);
		//		note_active.erase(it);
		if(it != note_active.end()){
			if(n_note->stop_offset == 0){
				it->stop_offset = 1;
			}else{
				it->stop_offset = n_note->stop_offset;
			}
		}
	}
}

void midi_parser::set_input(float* ptr){
	in = ptr;
}

void midi_parser::set_midi_uri(LV2_URID uri){
	midi_Event = uri;
}

/*
void midi_parser::process_midi_2(int nb_sample){
	uint8_t *data, bmask;
	uint32_t event_count = 0;

	LV2_Event_Buffer* midi_in = (LV2_Event_Buffer*)in;
	LV2_Event* event = (LV2_Event*)midi_in->data;
	uint8_t* tmp = (uint8_t*)midi_in->data;

	//cout << " process midi " << endl;

	while(event_count < midi_in->event_count){
		data = (uint8_t*)event + sizeof(LV2_Event);
		bmask = data[0] & 0xF0;
	//	cout << "event midi " << dec << data[0] << " bmask " << dec << bmask << endl;
	//	printf("event midi Ox%x\n", data[0]);
		switch(bmask){
			//note on
			case 0x90:
			{
				note* nt = new note(data[1], event->frames);
				note_on(nt);
				delete nt;
				//cout << "note on" << int(data[1]) << " " << event->frames << endl;
			}
			break;
			//note off
			case 0x80:
			{
				note* nt = new note(data[1], event->frames);
				nt->stop_offset = event->frames;
				note_off(nt);
				delete nt;
				//cout << "note off" << int(data[1]) << " " << event->frames << endl;
			}
			break;
			//control change & all note/sound off
			case 0xB0:
				switch(data[1]){
					//all note off
					case 123:
					//all sound off
					case 120:
						cout << "Test all note/sound off " << endl;
						note_active.clear();
					break;
					default:
						cout << "Control Change: " << data[1] << endl;
					break;
				}
			break;
			default:
				cout << "Unknown midi event" << endl;
				cout << "event midi " << int(data[0]) << " bmask " << int(bmask) <<  " val " << int(data[1]) << endl;
			break;
		}
		event_count++;
		//event += (sizeof(LV2_Event) + event->size + 7) &(~7);
		tmp += (sizeof(LV2_Event) + event->size + 7) &(~7);
		event = (LV2_Event*)tmp;
	}
}
					 */

void midi_parser::process_midi(int nb_sample){

	const uint8_t* data;
	LV2_Atom_Sequence* midi_in = (LV2_Atom_Sequence*)in;

	LV2_ATOM_SEQUENCE_FOREACH(midi_in, ev){
		if(ev->body.type == midi_Event){
			data = (const uint8_t*)LV2_ATOM_BODY(&ev->body);
			switch(data[0] & 0xF0){
				/*note on*/
				case 0x90:
				{
					note* nt = new note(data[1], ev->time.frames);
					note_on(nt);
					delete nt;
					//cout << "note on" << int(data[1]) << " " << event->frames << endl;
				}
				break;
				/*note off*/
				case 0x80:
				{
					note* nt = new note(data[1], ev->time.frames);
					nt->stop_offset = ev->time.frames;
					note_off(nt);
					delete nt;
					//cout << "note off" << int(data[1]) << " " << event->frames << endl;
				}
				break;
				/*control change & all note/sound off*/
				case 0xB0:
					switch(data[1]){
						/*all note off*/
						case 123:
							/*all sound off*/
						case 120:
							cout << "Test all note/sound off " << endl;
							//note_active.clear();
							break;
						default:
							cout << "Control Change: " << data[1] << endl;
							break;
					}
					break;
					default:
						cout << "Unknown midi event" << endl;
						cout << "event midi " << int(data[0]) <<  " val " << int(data[1]) << endl;
						break;
			}
		}
	}
}
