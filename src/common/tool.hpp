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

#ifndef TOOL_HPP
#define TOOL_HPP

#include <stdint.h>

#include "note.hpp"

void release_mod(note *nt, float* amp_mod_buff, uint32_t release_time, uint32_t nb_sample, uint32_t *end);

void smooth_filter(float* in, float* out, int nb_sample);

void smooth_filter(float* buff, int nb_sample);

#endif
