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

#include "LV2UI.hpp"

LV2UI::LV2UI()
	:lv2_ctrl(NULL),lv2_write(NULL){

}

void LV2UI::set_lv2_ctrl(LV2UI_Controller lc){
	lv2_ctrl = lc;
}

void LV2UI::set_lv2_write_fn(LV2UI_Write_Function lw){
	lv2_write = lw;
}

void LV2UI::lv2_send_curve(float* curve, uint32_t size){

    //this->lv2_write(lv2_ctrl, 2, size, 0, curve);
    //this->lv2_write(lv2_ctrl, 2, size, atom_vector_urid, curve);
//      toto = new float;
//      *toto = 1;
//      this->lv

}

void LV2UI::lv2_send_data(uint32_t index, double val){

	float value = val;
	if((lv2_write != NULL)&&(lv2_ctrl != NULL)){
		lv2_write(lv2_ctrl, index, sizeof(float), 0, &value);
	}
}

LV2WaveDraw::LV2WaveDraw(QWidget *parent, uint32_t tab_size)
	:TimeCurveDraw(parent, tab_size), LV2UI(){

	setTitle("Draw your Wave");
	enable_vol_knob();
}
void LV2WaveDraw::send_vol(double value){
	lv2_send_data(3, value);
}


LV2SpectrumDraw::LV2SpectrumDraw(QWidget *parent, uint32_t tab_size)
	:FreqCurveDraw(parent, tab_size), LV2UI(){

	setTitle("Draw your Spectrum");
	enable_vol_knob();
	enable_freq_shift_knob();
}

void LV2SpectrumDraw::send_vol(double value){
	lv2_send_data(3, value);
}

void LV2SpectrumDraw::send_freq(double value){
	lv2_send_data(2, value);
}


LV2SFilterDraw::LV2SFilterDraw(QWidget *parent, uint32_t tab_size)
	:FreqCurveDraw(parent, tab_size), LV2UI(){

	setTitle("Draw your Filter's Spectrum");
	enable_vol_knob();
	enable_freq_shift_knob();
}

void LV2SFilterDraw::send_vol(double value){
	lv2_send_data(3, value);
}

void LV2SFilterDraw::send_freq(double value){
	lv2_send_data(2, value);
}


LV2CFilterDraw::LV2CFilterDraw(QWidget *parent, uint32_t tab_size)
	:FreqCurveDraw(parent, tab_size), LV2UI(){

	setTitle("Draw your Filter's Spectrum");
	enable_vol_knob();
}

void LV2CFilterDraw::send_vol(double value){
	lv2_send_data(3, value);
}

void LV2CFilterDraw::send_freq(double value){

}

