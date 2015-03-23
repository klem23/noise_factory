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

#include <QMouseEvent>

#include "wave_picker.hpp"

WavePicker::WavePicker(QWidget *canvas, QWidget *plot, uint32_t tab_size):QwtPlotPicker((QwtPlotCanvas*)canvas){

    size = tab_size;
    x = new double[size];
    y = new double[size];

    ymin = 0;
    ymax = 1;

    curve = new QwtPlotCurve();
    curve->setRawSamples(x, y, size);
    curve->attach((QwtPlot*)plot);
}

WavePicker::~WavePicker(){
  delete x;
  delete y;
  delete curve;
}

void WavePicker::draw(){
     curve->setRawSamples(x, y, size);
}

void WavePicker::setBound(double min, double max){
    ymin = min;
    ymax = max;
}

double WavePicker::getGraph(uint32_t idx){
	if(idx < size){
		return y[idx];
	}else{
		return y[size - 1];
	}
}

float* WavePicker::getGraph(void){
	float* out = new float[size];

	if(out != NULL){
		for(uint32_t i = 0; i < size; i++)
		{
			out[i] = y[i];	
		}
	}

	return out;
}

uint32_t WavePicker::getSize(void){
	return size;
}

TimePicker::TimePicker(QWidget *canvas, QWidget *plot, uint32_t tab_size):WavePicker(canvas, plot, tab_size){
  fill_tables();
}

TimePicker::~TimePicker(){

}

void TimePicker::widgetMousePressEvent(QMouseEvent *ev){
    QPointF qpf = invTransform(ev->pos());

    uint32_t abs = qpf.x();
    double ord = qpf.y();

    if(abs < size )
    {
        if(ord < ymin)
            ord = ymin;
        if(ord > ymax)
            ord = ymax;

        y[abs] = ord;
    }

    //cout << "mousevent x :"<< abs << " y:"<< ord << endl;
}

void TimePicker::widgetMouseMoveEvent(QMouseEvent *ev){
    QPointF qpf = invTransform(ev->pos());

    uint32_t abs = qpf.x();
    double ord = qpf.y();

    if(abs < size )
    {
        if(ord < ymin)
            ord = ymin;
        if(ord > ymax)
            ord = ymax;

        y[abs] = ord;
    }

    cout << "mousevent x :"<< abs << " y:"<< ord << endl;
}

void TimePicker::fill_tables(){

    float freq = 1.0 / size;

    for(uint32_t i = 0; i < size; i++){
        x[i] = i;
        y[i] = sinf(2 * M_PI * freq * i);
    }

}

FreqPicker::FreqPicker(QWidget *canvas, QWidget *plot, uint32_t tab_size, uint8_t gtype /* = SPECTRUM*/):WavePicker(canvas, plot, tab_size){
  type = gtype;
  fill_tables();
}


void FreqPicker::widgetMousePressEvent(QMouseEvent *ev){
    QPointF qpf = invTransform(ev->pos());

    /*get next pixel coord*/
    QPoint next_qp = ev->pos();
    next_qp.setX(next_qp.x() + 1);
    QPointF next_qpf = invTransform(next_qp);
    uint32_t next_abs = (next_qpf.x() - 100.0)/5.0;
    double next_ord = next_qpf.y();

    if(next_ord < ymin)
        next_ord = ymin;
    if(next_ord > ymax)
        next_ord = ymax;

    /*get current pixel coord*/
    uint32_t abs = (qpf.x() - 100.0)/5.0;
    double ord = qpf.y();

    if(ord < ymin)
        ord = ymin;
    if(ord > ymax)
        ord = ymax;

    /*update curve values*/
    if(abs < size){
        uint32_t max;
        if(next_abs < size){
            max = next_abs;
        }else{
            max = size;
        }
        double coeff = ((next_ord - ord)/(float)(next_abs - abs));
        double orig = ord - coeff * abs;
        for(uint32_t i = abs; i < max; i++){
            //y[i] = ord;
            y[i] = coeff * i + orig;
        }
    }

    //cout << "mousevent x :"<< abs << " y:"<< ord << endl;
}

void FreqPicker::widgetMouseMoveEvent(QMouseEvent *ev){
    QPointF qpf = invTransform(ev->pos());

    /*get next pixel coord*/
    QPoint next_qp = ev->pos();
    next_qp.setX(next_qp.x() + 1);
    QPointF next_qpf = invTransform(next_qp);
    uint32_t next_abs = (next_qpf.x() - 100.0)/5.0;
    double next_ord = next_qpf.y();

    if(next_ord < ymin)
        next_ord = ymin;
    if(next_ord > ymax)
        next_ord = ymax;

    /*get current pixel coord*/
    uint32_t abs = (qpf.x() - 100.0)/5.0;
    double ord = qpf.y();

    if(ord < ymin)
        ord = ymin;
    if(ord > ymax)
        ord = ymax;

    /*update curve values*/
    if(abs < size){
        uint32_t max;
        if(next_abs < size){
            max = next_abs;
        }else{
            max = size;
        }
        double coeff = ((next_ord - ord)/(float)(next_abs - abs));
        double orig = ord - coeff * abs;
        for(uint32_t i = abs; i < max; i++){
            //y[i] = ord;
            y[i] = coeff * i + orig;
        }
    }

    //cout << "mousevent x :"<< abs << " y:"<< ord << endl;
}

void FreqPicker::fill_tables(){

  if(type == SPECTRUM){
      uint32_t i = 0;

      while(i < size){
          x[i] = i * 5 + 100;
          y[i] = 0;
          i++;
        }
      y[23] = 1;
      y[2323] = 1;

    }else{

      uint32_t i = 0;

      while(i < size/2){
          x[i] = i * 5 + 100;
          y[i] = 1;
          i++;
        }

      while(i < size){
          x[i] = i * 5 + 100;
          y[i] = 0;
          i++;
        }
    }
}

FreqPicker::~FreqPicker(){

}
