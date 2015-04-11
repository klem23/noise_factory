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


void LV2UI::set_lv2_ctrl(LV2UI_Controller lc){
	//if(lc != NULL){
	lv2_ctrl = lc;
	//}
}

void LV2UI::set_lv2_write_fn(LV2UI_Write_Function lw){
	//if(lw != NULL){
	lv2_write = lw;
	//}
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
