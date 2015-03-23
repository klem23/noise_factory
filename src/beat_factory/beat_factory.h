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

#ifndef BEAT_FACTORY_H
#define BEAT_FACTORY_H

#include <stdbool.h>
#include "osc.h"

typedef struct{
	/*I/O*/
	float* midi_in;
	float* audio_out;
	
	char active;
	float sample_rate;

	osc* osc1;
	float* osc2_active;
	osc* osc2;

	float* temp_buff;
}BF;


#endif 
