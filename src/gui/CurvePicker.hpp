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

#ifndef CURVE_PICKER_HPP
#define CURVE_PICKER_HPP

#include <stdint.h>

#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_picker.h>

#define WAVE      0
#define SPECTRUM  1
#define FILTER    2
#define CFILTER   3


using namespace std;

class CurvePicker : public QwtPlotPicker
{
protected:
    QwtPlotCurve *curve;
    double *x, *y;
    uint32_t size;
    double ymin, ymax;

    virtual void fill_tables() = 0;
    virtual void widgetMouseMoveEvent(QMouseEvent *ev) = 0;
    virtual void widgetMousePressEvent (QMouseEvent *ev) = 0;

public:
    CurvePicker(QWidget *canvas, QWidget *plot, uint32_t tab_size);
    ~CurvePicker();
    void draw();
    void setBound(double min, double max);
    double getGraph(uint32_t idx);
    float* getGraph(void);
    uint32_t getSize(void);
};


class TimePicker : public CurvePicker
{
private:
    void fill_tables();
    void widgetMouseMoveEvent(QMouseEvent *ev);
    void widgetMousePressEvent (QMouseEvent *ev);

public:
    TimePicker(QWidget *canvas, QWidget *plot, uint32_t tab_size);
    ~TimePicker();
};


class FreqPicker : public CurvePicker
{
private:
    uint8_t type;
    void fill_tables();
    void widgetMouseMoveEvent(QMouseEvent *ev);
    void widgetMousePressEvent (QMouseEvent *ev);

public:
    FreqPicker(QWidget *canvas, QWidget *plot, uint32_t tab_size, uint8_t gtype = SPECTRUM);
    ~FreqPicker();
};

#endif // CURVE_PICKER_HPP
