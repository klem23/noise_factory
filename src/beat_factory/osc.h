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

#ifndef OSC_H
#define OSC_H

/*oscillator simulator for signal generation*/

typedef struct{
	int osc_inc;
	int type;
	float* lv2_type;
	float* freq_start;
	float* freq_end;
	float* amp_env;
	float* freq_env;
	float* vol;
}osc;

/*percussive enveloppe for signal*/
void freq_env(osc* osci, int nb, float srate, float* data);
void amp_env(osc* osci, int nb, float srate, float* data);
void release_prev(float* out, int stop, float srate);

/*signal generation*/
void osc_sin(osc* osci, int nb, float srate, float* data, float* out);
void osc_square(osc* osci, int nb, float srate, float* data, float* out);
void osc_saw_up(osc* osci, int nb, float srate, float* data, float* out);
void osc_saw_down(osc* osci, int nb, float srate, float* data, float* out);
void osc_triangle(osc* osci, int nb, float srate, float* data, float* out);

#endif
