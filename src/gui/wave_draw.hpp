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

#ifndef WAVEDRAW_H
#define WAVEDRAW_H


#include <stdint.h>

#include <QWidget>
#include <QMouseEvent>
#include <QPushButton>
#include <QLayout>

#include <qwt/qwt_knob.h>
#include <qwt/qwt_plot.h>

#include "wave_picker.hpp"

#ifdef LV2_GUI
        #include "lv2/lv2plug.in/ns/extensions/ui/ui.h"
#endif

#ifdef OSC
        #include <lo/lo.h>
#endif

class WaveDraw : public QWidget
{
	Q_OBJECT

private:
	uint8_t type;

	QwtPlot *graph;
	WavePicker *wp;

	QPushButton *send_btn;
	QwtKnob *freq_btn;
	QwtKnob *vol_btn;

	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);

#ifdef LV2_GUI
	LV2UI_Controller lv2_ctrl;
	LV2UI_Write_Function lv2_write;
	float* data;


	void lv2_send_data(uint32_t index, double data);
#endif


#ifdef OSC
	lo_address osc_addr;
#endif


public:
	WaveDraw(QWidget *parent, uint32_t tab_size, uint8_t gtype = WAVE);
	~WaveDraw();
	void draw();

	public slots:
	void send_graph(void);
	void send_vol(double value);
	void send_freq(double value);

#ifdef LV2_GUI
	void set_lv2_ctrl(LV2UI_Controller lc);
	void set_lv2_write_fn(LV2UI_Write_Function lw);
#endif

#ifdef OSC
	void osc_send_data(uint32_t index, double data);
#endif
};

#endif // WAVEDRAW_H
