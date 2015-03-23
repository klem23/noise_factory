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

#ifndef FENV_HPP
#define FENV_HPP

#include "env.hpp"

typedef struct{
	float* active;
	float* A_up;
	float* A_time;
	float* D_on;
	float* D_time;
	float* R_up;
	float* R_time;
	float* amt;
}fenv_param;


class fenv : public env {
	private:
		bool A_UP; /* attack up or down*/
		bool D_ON; /*decay on or off */
		bool R_UP; /* release up or down*/

		void calc();

	public:
		fenv(uint32_t s_rate);
		~fenv();

		void check_param(fenv_param* fp);

		void process(float* out, note* nt, uint32_t stop);

};



#endif /* FENV_HPP */
