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

#include <iostream>

#include "OSCUI.hpp"

OSCUI::OSCUI(uint32_t port_t):port(port_t){

    char str[12];
    sprintf(str, "%d", port);
    osc_addr = lo_address_new(NULL, str);

}


void OSCUI::osc_send_curve(float* curve, uint32_t size){
    cout << "#klem test osc size :" << size << endl;

    lo_blob data = lo_blob_new(size*sizeof(float), curve);
    if(lo_send(osc_addr, "/wave/table", "b", data) == -1) {
            std::cout << "OSC error : " << lo_address_errno(osc_addr) << " : " << lo_address_errstr(osc_addr) << std::endl;
    }else{
            std::cout << "Curve send by OSC message" << std::endl;
    }
}

void OSCUI::osc_send_data(uint32_t index, double val){

	string path;
	float value = val;

	if(index == 2) path = "/wave/freq_shift";
	else if(index == 3) path = "/wave/volume";

	if(lo_send(osc_addr, path.c_str(), "f", value) == -1) {
			std::cout << "OSC error " << lo_address_errno(osc_addr) << " : " << lo_address_errstr(osc_addr) << std::endl;
    }else{
            std::cout << "OSC message send" << endl;
    }
}


OSCWaveDraw::OSCWaveDraw(QWidget *parent, uint32_t tab_size)
	:TimeCurveDraw(parent, tab_size), OSCUI(2323){

	setTitle("Draw your Wave");
	enable_vol_knob();
}

void OSCWaveDraw::send_vol(double value){
	osc_send_data(3, value);
}


void OSCWaveDraw::send_graph(float* curve, uint32_t size){
	osc_send_curve(curve, size);
}

OSCSpectrumDraw::OSCSpectrumDraw(QWidget *parent, uint32_t tab_size)
	:FreqCurveDraw(parent, tab_size), OSCUI(2324){

	setTitle("Draw your Spectrum");
	enable_freq_shift_knob();
	enable_vol_knob();
}

void OSCSpectrumDraw::send_vol(double value){
	osc_send_data(3, value);
}

void OSCSpectrumDraw::send_freq(double value){
	osc_send_data(2, value);
}

void OSCSpectrumDraw::send_graph(float* curve, uint32_t size){
	osc_send_curve(curve, size);
}



OSCSFilterDraw::OSCSFilterDraw(QWidget *parent, uint32_t tab_size)
	:FreqCurveDraw(parent, tab_size), OSCUI(2325){

	setTitle("Draw your Filter's Spectrum");
	enable_freq_shift_knob();
	enable_vol_knob();
}

void OSCSFilterDraw::send_vol(double value){
	osc_send_data(3, value);
}

void OSCSFilterDraw::send_freq(double value){
	osc_send_data(2, value);
}

void OSCSFilterDraw::send_graph(float* curve, uint32_t size){
	osc_send_curve(curve, size);
}


OSCCFilterDraw::OSCCFilterDraw(QWidget *parent, uint32_t tab_size)
	:FreqCurveDraw(parent, tab_size), OSCUI(2326){

	setTitle("Draw your Filter's Spectrum");
	enable_vol_knob();
}

void OSCCFilterDraw::send_vol(double value){
	osc_send_data(3, value);
}

void OSCCFilterDraw::send_freq(double value){

}

void OSCCFilterDraw::send_graph(float* curve, uint32_t size){
	osc_send_curve(curve, size);
}
