/*
	This file is part of thelo_address osc_addr; Noise Factory
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

#ifndef OSC_UI_HPP
#define OSC_UI_HPP

#include <stdint.h>
#include <lo/lo.h>

#include "CurveDraw.hpp"

class OSCUI{

private:
	lo_address osc_addr;
	uint32_t   port;

protected:
	OSCUI(uint32_t port_t = 2323);
	void osc_send_curve(float* curve, uint32_t size);
	void osc_send_data(uint32_t index, double data);

};


class OSCWaveDraw : public TimeCurveDraw, public OSCUI{
private:
	void send_vol(double value);
	void send_graph(float* curve, uint32_t size);

public:
	OSCWaveDraw(QWidget *parent, uint32_t tab_size);

};

class OSCSpectrumDraw : public FreqCurveDraw, public OSCUI{
private:
	void send_vol(double value);
	void send_freq(double value);
	void send_graph(float* curve, uint32_t size);

public:
	OSCSpectrumDraw(QWidget *parent, uint32_t tab_size);
};

class OSCSFilterDraw : public FreqCurveDraw, public OSCUI{
private:
	void send_vol(double value);
	void send_freq(double value);
	void send_graph(float* curve, uint32_t size);

public:
	OSCSFilterDraw(QWidget *parent, uint32_t tab_size);
};

class OSCCFilterDraw : public FreqCurveDraw, public OSCUI{
private:
	void send_vol(double value);
	void send_freq(double value);
	void send_graph(float* curve, uint32_t size);

public:
	OSCCFilterDraw(QWidget *parent, uint32_t tab_size);
};


#endif // OSC_UI_HPP
