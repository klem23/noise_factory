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

#ifndef CURVE_DRAW_H
#define CURVE_DRAW_H


#include <stdint.h>

#include <QWidget>
#include <QMouseEvent>
#include <QPushButton>
#include <QLayout>

#include <qwt/qwt_knob.h>
#include <qwt/qwt_plot.h>

#include "CurvePicker.hpp"

class CurveDraw : public QWidget
{
	Q_OBJECT

protected:
	uint8_t type;
	uint32_t size;

    QHBoxLayout *lay;
    QVBoxLayout *vlay;

	QwtPlot *graph;
	CurvePicker *wp;

	QPushButton *send_btn;
	QwtKnob *freq_btn;
	QwtKnob *vol_btn;

	virtual void send_vol(double value) = 0;

	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);

public:
	CurveDraw(QWidget *parent, uint32_t tab_size, uint8_t gtype = WAVE);
	~CurveDraw();
	void draw();
	void setTitle(QString ttl);
	void enable_vol_knob(void);

public slots:
	void send_graph(void);

};


class TimeCurveDraw : public CurveDraw{
private:

public:
	TimeCurveDraw(QWidget *parent, uint32_t tab_size);
};

class FreqCurveDraw : public CurveDraw{
private:
	virtual void send_freq(double value) = 0;

public:
	FreqCurveDraw(QWidget *parent, uint32_t tab_size);

	void enable_freq_shift_knob(void);
};

#endif // CURVE_DRAW_H
