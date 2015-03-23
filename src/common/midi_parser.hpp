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

#ifndef MIDI_PARSER_HPP
#define MIDI_PARSER_HPP

#include <vector>
#include <list>

#include "note.hpp"

#include "lv2/lv2plug.in/ns/ext/urid/urid.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"


class midi_parser{
	protected:
		uint32_t srate;
		float freq_table[127];
		list<note> note_active;
		list<note>::iterator it;
		float * in;

		LV2_URID midi_Event;

		void note_on(note* n_note);
		void note_off(note* n_note);

	public:
		midi_parser(uint32_t sampling_rate = 44100);

		void process_midi(int nb_sample);
		void process_midi_2(int nb_sample);

		void set_input(float* ptr);
		void set_midi_uri(LV2_URID uri);
};
#endif
