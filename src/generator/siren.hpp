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

#ifndef SIREN_HPP
#define SIREN_HPP

/* mi-femme mi-thon */

//#include <samplerate.h>

//#include "midi_parser.hpp"
#include "osc.hpp"


typedef struct {
	float* time_up;
	float* freq_up;
	float* time_down;
	float* freq_down;
	float* vol;
	bool   pitched;
}si_param;

class siren : public osc{
private:

	float time_up;
	float freq_up;
	float time_down;
	float freq_down;
	float vol;
	bool  pitched;

	void note_interceptor(void);
	void freq_interceptor(float* freq_mod_buff, int nb_sample);

public:

	siren(uint32_t sampling_rate);		
	~siren(void);		

	void si_check_param(si_param* spi, osc_param* op);
	void process(int nb_sample);

};
#endif
