/*
	This file is part of the Noise Factory
	Copyright (C) 2015 Clément Guedez <klem.dev@gmail.com>


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

#ifndef ZP_DIAG_FILTER_HPP
#define ZP_DIAG_FILTER_HPP


#include <cmath>
#include <complex.h>
#include "filter.hpp"



typedef struct{
	float *band_type;
	float *filter_type;
	float *order;
	float *cutoff;
	float *rp;
}filter_param;



class zp_diag_filter : public recursive_filter{

	private:
		enum filter_type{
			chebychev,
			butterworth,
			elliptic
		};

		uint8_t zp_nb;
		float rp;
		enum filter_type f_type;

		double complex *zero, *pole; 
		double *x0, *x1, *x2, *y1, *y2;
		double *a0, *a1, *a2, *b1, *b2;

		/*coeff calculus*/
		void create_intern_tab(uint8_t zp_num);
		void destroy_intern_tab(void);
		void calcul_diag(void);
		void calcul_ellipse(void);
		void calcul_z_trans(void);
		void calcul_coeff(void);
		void combine_coeff(void);

		/*parameter change*/
		void set_band_type(uint8_t type);
		void set_filter_type(enum filter_type type);
		void set_order(uint8_t order_t);
		void set_fc(float cutoff);
		void set_rp(float rp_p);


	public:
		zp_diag_filter(uint32_t s_rate, uint8_t zp_num);
		~zp_diag_filter(void);


		void check_param(filter_param* fp);

};

#endif
