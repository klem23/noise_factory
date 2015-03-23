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


zp_diag_filter::zp_diag_filter(uint32_t s_rate, uint8_t zp_num):recursive_filter(s_rate),rp(0.5){

	rp = 0.5;

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

	pole = new float complex[zp_nb];

/*
	if(band_type != LOW_PASS){
		zero = new float complex[zp_nb];
	}
*/
	x0 = new float[zp_nb];
	x1 = new float[zp_nb];
	x2 = new float[zp_nb];
	y1 = new float[zp_nb];
	y2 = new float[zp_nb];

	a0 = new float[zp_nb];
	a1 = new float[zp_nb];
	a2 = new float[zp_nb];
	b1 = new float[zp_nb];
	b2 = new float[zp_nb];

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
		float pole_real = - cos( M_PI/(zp_nb*2) + (i-1) * M_PI/zp_nb);
		float pole_img = sin( M_PI/(zp_nb*2) + (i-1) * M_PI/zp_nb);
		pole[i-1] = pole_real + I * pole_img; 
		printf("pole location R %f, I %f\n", pole_real, pole_img);
	} 

}

void zp_diag_filter::calcul_ellipse(void){
	if(f_type == butterworth){
		return;
	}

	/*change point on s-plane to get an ellipse*/
	float es = sqrtf( powf( (100.0/(100.0 - rp)), 2) - 1.0);
	float vx = (1.0/zp_nb)*logf( (1.0/es) + sqrtf( 1.0/(es*es) + 1.0));
	float kx = (1.0/zp_nb)*logf( (1.0/es) + sqrtf( 1.0 / (es*es) - 1.0));
	kx = (expf(kx) + expf(-kx))/2;
	printf("ellipse coeff es %f vx %f kx %f\n", es, vx, kx);

	for(int i = 1; i <= zp_nb/2; i++){
		float pole_real = creal(pole[i-1])*((expf(vx)-expf(-vx))/2.0)/kx;
		float pole_img = cimag(pole[i-1])*((expf(vx)+expf(-vx))/2.0)/kx;
		pole[i-1] = pole_real + I * pole_img;
		printf("pole location on ellipse R %f, I %f\n", pole_real, pole_img);
	}
}

void zp_diag_filter::calcul_z_trans(void){
	float m, d;

	float T = 2*tanf(0.5);
	float T2 = T*T;

	for(int i = 1; i <= zp_nb/2; i++){
		m = powf(creal(pole[i-1]),2) + powf(cimag(pole[i-1]),2);
		d = 4 - 4*creal(pole[i-1])*T + m*T2; 
		printf("z_trans T %f, T2 %f, m %f, d %f\n", T, T2, m, d);


		x0[i-1] = T2/d;
		x1[i-1] = 2*x0[i-1];
		x2[i-1] = x0[i-1];

		y1[i-1] = (8 - 2*m*T2) / d;
		y2[i-1] = (-4 - 4*creal(pole[i-1])*T - m*T2) / d;
		printf("run %d: %d pole's filter z_trans x0 %f x1 %f x2 %f y1 %f y2 %f \n",
					i-1, zp_nb , x0[i-1], x1[i-1], x2[i-1], y1[i-1], y2[i-1]);
	}
}

void zp_diag_filter::calcul_coeff(void){

	float d;
	float K;

	float W = 2 * M_PI * fc;
	printf("coeff omega : %f\n", W);


	switch(band_type){ 	
		case LOW_PASS:
			K =  sin(0.5 - W/2) / sin(0.5 + W/2);
		break;
		case HIGH_PASS:
			K = - cos(W/2 + 0.5) / cos(W/2 - 0.5);
		break;
	}

	float K2 = K*K; 

	for(int i = 1; i <= zp_nb/2; i++){
		d = 1 + y1[i-1]*K - y2[i-1] * K2; 

		a0[i-1] = (x0[i-1] - x1[i-1]*K + x2[i-1]*K2) /d;
		a1[i-1] = (-2*x0[i-1]*K + x1[i-1] + x1[i-1]*K2 - 2*x2[i-1]*K) /d;
		a2[i-1] = (x0[i-1]*K2 - x1[i-1]*K + x2[i-1]) / d;
		b1[i-1] = (2*K + y1[i-1] + y1[i-1]*K2 - 2*y2[i-1]*K) /d;
		b2[i-1] = (-K2 - y1[i-1]*K + y2[i-1]) /d;

		if(band_type == HIGH_PASS){
			a1[i-1] = - a1[i-1];
			b1[i-1] = - b1[i-1];
		}

		printf("run %d: %d pole's filter coeff a0 %f a1 %f a2 %f b1 %f b2 %f \n",
					i-1, zp_nb , a0[i-1], a1[i-1], a2[i-1], b1[i-1], b2[i-1]);

	}
}

void zp_diag_filter::combine_coeff(void){
/*
	float* temp_a = new float[order+3];
	float* temp_b = new float[order+3];

	memset(temp_a, 0, (order+3)*sizeof(float));
	memset(temp_b, 0, (order+3)*sizeof(float));
*/

	float temp_a[22];
	float temp_b[22];
//	float a[22];
//	float b[22];

	memset(temp_a, 0, 22*sizeof(float));
	memset(temp_b, 0, 22*sizeof(float));

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
		printf("resulting coeff a_coeff[%d] = %f ## b_coeff[%d] = %f \n", i, a_coeff[i], i, b_coeff[i]);
	}

	/*normalize gain*/
	float sa = 0;
	float sb = 0;

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

	float gain = sa / (1-sb);

	for(int i = 0; i <= order; i++){
		//a_coeff[i] /= gain;
		//a_coeff[i] /= 3;
	}

	for(int i = 0; i <= order; i++){
		printf("resulting gain coeff a[%d] = %f ## b[%d] = %f \n", i, a_coeff[i], i, b_coeff[i]);
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
