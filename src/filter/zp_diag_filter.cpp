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

#include "zp_diag_filter.hpp"

#include <iostream>


zp_diag_filter::zp_diag_filter(uint32_t s_rate, uint8_t zp_num):recursive_filter(s_rate),rp(0.5){

	if(zp_num == 0){
		zp_nb = order;
	}else{
		zp_nb = zp_num;
	}

	pole = NULL;

	x0 = NULL;
	x1 = NULL;
	x2 = NULL;
	y1 = NULL;
	y2 = NULL;

	a0 = NULL;
	a1 = NULL;
	a2 = NULL;
	b1 = NULL;
	b2 = NULL;

	//create_intern_tab(zp_nb);
	set_order(zp_nb);
}

zp_diag_filter::~zp_diag_filter(void){
	destroy_intern_tab();
}

void zp_diag_filter::create_intern_tab(uint8_t zp_num){

	if(zp_num == 0){
		zp_nb = order;
	}else{
		zp_nb = zp_num;
	}

	pole = new double complex[zp_nb];

/*
	if(band_type != LOW_PASS){
		zero = new float complex[zp_nb];
	}
*/
	x0 = new double[zp_nb];
	x1 = new double[zp_nb];
	x2 = new double[zp_nb];
	y1 = new double[zp_nb];
	y2 = new double[zp_nb];

	a0 = new double[zp_nb];
	a1 = new double[zp_nb];
	a2 = new double[zp_nb];
	b1 = new double[zp_nb];
	b2 = new double[zp_nb];

}


void zp_diag_filter::destroy_intern_tab(void){
	
	delete pole;

	delete x0;
	delete x1;
	delete x2;
	delete y1;
	delete y2;

	delete a0;
	delete a1;
	delete a2;
	delete b1;
	delete b2;
}


void zp_diag_filter::calcul_diag(void){
	/*calculate point on s-plane*/
	for(int i = 1; i <= zp_nb/2; i++){
		double pole_real = - cos( M_PI/(zp_nb*2.0) + (i-1.0) * M_PI/zp_nb);
		double pole_img = sin( M_PI/(zp_nb*2.0) + (i-1.0) * M_PI/zp_nb);
		pole[i-1] = pole_real + I * pole_img; 
		std::cout << "pole location R " << pole_real << " I " << pole_img << std::endl;
	} 

}

void zp_diag_filter::calcul_ellipse(void){
	if(f_type == butterworth){
		return;
	}

	/*change point on s-plane to get an ellipse*/
	double es = sqrt( pow( (100.0/(100.0 - rp)), 2) - 1.0);
	double vx = (1.0/zp_nb)*log( (1.0/es) + sqrt( 1.0/(es*es) + 1.0));
	double kx = (1.0/zp_nb)*log( (1.0/es) + sqrt( 1.0 / (es*es) - 1.0));
	kx = (exp(kx) + exp(-kx))/2.0;
	std::cout << "ellipse coeff es " << es << " vx " << vx << " kx " << kx << std::endl;

	for(int i = 1; i <= zp_nb/2; i++){
		double pole_real = creal(pole[i-1])*((exp(vx)-exp(-vx))/2.0)/kx;
		double pole_img = cimag(pole[i-1])*((exp(vx)+exp(-vx))/2.0)/kx;
		pole[i-1] = pole_real + I * pole_img;
		std::cout << "pole location on ellipse R " << pole_real << " I " << pole_img << std::endl;
	}
}

void zp_diag_filter::calcul_z_trans(void){
	double m, d;

	double T = 2*tanf(0.5);
	double T2 = T*T;

	for(int i = 1; i <= zp_nb/2; i++){
		m = pow(creal(pole[i-1]),2) + pow(cimag(pole[i-1]),2);
		d = 4.0 - 4.0*creal(pole[i-1])*T + m*T2; 
		std::cout << "z_trans T " << T << " T2 " << T2 << " m " << m << " d " << d << std::endl;


		x0[i-1] = T2/d;
		x1[i-1] = 2.0*x0[i-1];
		x2[i-1] = x0[i-1];

		y1[i-1] = (8.0 - 2.0*m*T2) / d;
		y2[i-1] = (-4.0 - 4.0*creal(pole[i-1])*T - m*T2) / d;
		std::cout << "run " << i-1 << " : " << zp_nb << "pole's filter z_trans x0 "
				<<  x0[i-1] << " x1 " <<  x1[i-1] << " x2 " << x2[i-1] << " y1 " << y1[i-1] << " y2 " << y2[i-1] << std::endl;
	}
}

void zp_diag_filter::calcul_coeff(void){

	double d;
	double K;

	double W = 2.0 * M_PI * fc;
	std::cout << "coeff omega : " << W << std::endl;


	switch(band_type){ 	
		case LOW_PASS:
			K =  sin(0.5 - W/2.0) / sin(0.5 + W/2.0);
		break;
		case HIGH_PASS:
			K = - cos(W/2.0 + 0.5) / cos(W/2.0 - 0.5);
		break;
	}

	double K2 = K*K; 

	for(int i = 1; i <= zp_nb/2; i++){
		d = 1.0 + y1[i-1]*K - y2[i-1] * K2; 

		a0[i-1] = (x0[i-1] - x1[i-1]*K + x2[i-1]*K2) /d;
		a1[i-1] = (-2.0*x0[i-1]*K + x1[i-1] + x1[i-1]*K2 - 2.0*x2[i-1]*K) /d;
		a2[i-1] = (x0[i-1]*K2 - x1[i-1]*K + x2[i-1]) / d;
		b1[i-1] = (2.0*K + y1[i-1] + y1[i-1]*K2 - 2.0*y2[i-1]*K) /d;
		b2[i-1] = (-K2 - y1[i-1]*K + y2[i-1]) /d;

		if(band_type == HIGH_PASS){
			a1[i-1] = - a1[i-1];
			b1[i-1] = - b1[i-1];
		}

		std::cout << "run " << i-1 << " : " << zp_nb << "pole's filter coeff a0 "
				<<  a0[i-1] << " a1 " <<  a1[i-1] << " a2 " << a2[i-1] << " b1 " << b1[i-1] << " b2 " << b2[i-1] << std::endl;
	}
}

void zp_diag_filter::combine_coeff(void){
/*
	float* temp_a = new float[order+3];
	float* temp_b = new float[order+3];

	memset(temp_a, 0, (order+3)*sizeof(float));
	memset(temp_b, 0, (order+3)*sizeof(float));
*/

	double temp_a[22];
	double temp_b[22];
//	float a[22];
//	float b[22];

	memset(temp_a, 0, 22*sizeof(double));
	memset(temp_b, 0, 22*sizeof(double));

	temp_a[2] = 1;
	temp_b[2] = 1;
	
	for(int j = 0; j < order/2 ; j++){
		//for(int i = order+2; i >= 2; i--){
		for(int i = 22; i >= 2; i--){
			temp_a[i] = a0[j]*temp_a[i] + a1[j]*temp_a[i-1] + a2[j]*temp_a[i-2];
			temp_b[i] = temp_b[i] - b1[j]*temp_b[i-1] - b2[j]*temp_b[i-2];
		}
	}

	temp_b[2] = 0;

	for(int i = 0; i <= order; i++){
		a_coeff[i] = temp_a[i+2];
		b_coeff[i] = - temp_b[i+2];
	}


/*original calculus*/
/*
	for(int j = 0; j < order/2 ; j++){
		//for(int i = 0; i <= order+2; i++){
		for(int i = 0; i < 22; i++){
			temp_a[i] = a[i];
			temp_b[i] = b[i];
		}

		//for(int i = 2; i <= order+2; i++){
		for(int i = 2; i < 22; i++){
			a[i] = a0[j]*temp_a[i] + a1[j]*temp_a[i-1] + a2[j]*temp_a[i-2];
			b[i] = temp_b[i] - b1[j]*temp_b[i-1] - b2[j]*temp_b[i-2];
		}
	}
	b[2] = 0;
	for(int i = 0; i <= order; i++){
		a_coeff[i] = a[i+2];
		b_coeff[i] = -b[i+2];
		a[i] = a[i+2];
		b[i] = -b[i+2];
	}
*/
	for(int i = 0; i <= order; i++){
		std::cout << "resulting coeff a_coeff[" << i << "] = " << a_coeff[i] << " ## b_coeff[" << i << "] = " << b_coeff[i] << std::endl;
	}

	/*normalize gain*/
	double sa = 0;
	double sb = 0;

	switch(band_type){ 	
		case LOW_PASS:
			//for(int i = 0; i <= order; i++){
			for(int i = 0; i < 20; i++){
				sa += temp_a[i+2];
				sb += -temp_b[i+2];
			}
			break;
		case HIGH_PASS:
			for(int i = 0; i <= order; i++){
				sa += a_coeff[i]*pow(-1, i);
				sb += b_coeff[i]*pow(-1, i);
			}
			break;
	 }

	double gain = sa / (1-sb);

	for(int i = 0; i <= order; i++){
		//a_coeff[i] /= gain;
		//a_coeff[i] /= 3;
	}

	for(int i = 0; i <= order; i++){
		std::cout << "resulting gain coeff a[" << i << "] = " << a_coeff[i] << " ## b[" << i << "] = " << b_coeff[i] << std::endl;
	}
}


void zp_diag_filter::set_band_type(uint8_t type){

	f_set_band_type(type);

	calcul_coeff();
	combine_coeff();
}

void zp_diag_filter::set_filter_type(enum filter_type type){

	f_type = type;

	calcul_diag();
	if(f_type != butterworth){
		calcul_ellipse();
	}
	calcul_z_trans();
	calcul_coeff();
	combine_coeff();
}

void zp_diag_filter::set_order(uint8_t order_t){

	rf_set_order(order_t);

	destroy_intern_tab();
	create_intern_tab(order_t);

	calcul_diag();
	if(f_type != butterworth){
		calcul_ellipse();
	}
	calcul_z_trans();
	calcul_coeff();
	combine_coeff();
}


void zp_diag_filter::set_fc(float cutoff){
	f_set_fc(cutoff);

	calcul_coeff();
	combine_coeff();
}

void zp_diag_filter::set_rp(float rp_p){
	rp = rp_p;

	if(f_type == butterworth){
		return;
	}

	calcul_ellipse();
	calcul_z_trans();
	calcul_coeff();
	combine_coeff();
}


void zp_diag_filter::check_param(filter_param* fp){


	if((uint8_t)*(fp->band_type) != band_type){
		set_band_type(*(fp->band_type));
	}

	if((enum filter_type)*(fp->filter_type) != f_type){
		set_filter_type((enum filter_type)*(fp->filter_type));
	}

	if((uint8_t)*(fp->order) != order){
		set_order(*(fp->order));
	}


	if((uint32_t)*(fp->cutoff) != (uint32_t)get_fc()){
		set_fc(*(fp->cutoff));
	}

	if((uint32_t)*(fp->rp) != (uint32_t)rp){
		set_rp(*(fp->rp));
	}

}
