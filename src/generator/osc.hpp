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

#ifndef OSC_HPP
#define OSC_HPP

#include "midi_parser.hpp"

#include "mod.hpp"
#include "env.hpp"
#include "lfo.hpp"
#include "perc.hpp"


using namespace std;

typedef struct{
	float *type;
	float *freq;
	float *phase;
	float *amp;
	env_param fe;
	lfo_param fl;
	env_param ae;
	lfo_param al;
	perc_param pe;
}osc_param;


class osc : public midi_parser{
private:

	/*parameter*/
	uint8_t type;
	float freq;
	float phase;
	float amp;


	uint32_t release_time; /*value in sample*/
	uint32_t anticlip_time; /*value in sample*/
	float freq_adj;

	/*uint32_t offset; //for monophonic use*/


	float *out;

	/*modulation*/
	float* freq_mod_buff;
	float* amp_mod_buff;
	env* freq_env;
	lfo* freq_lfo;
	env* amp_env;
	lfo* amp_lfo;
	perc* perc_env;

	/*signal generation*/
	void sinus(note* nt, uint32_t nb_sample);
	void square(note* nt, uint32_t nb_sample);
	void saw_up(note* nt, uint32_t nb_sample);
	void saw_down(note* nt, uint32_t nb_sample);
	void triangle(note* nt, uint32_t nb_sample);

	void release_mod(note* nt, uint32_t* end, uint32_t nb_sample);
	void modulation(note* nt, uint32_t nb_sample);

	void process_audio(int nb_sample);

public:
	osc(uint32_t s_rate = 44100);
	//osc(uint32_t s_rate = 44100, uint8_t type_t, float freq_t, float phase_t, float amp_t);
	~osc(void);

	void process(int nb_sample);
	void set_output(float* ptr);
	void check_param(osc_param *op);
};

#endif
