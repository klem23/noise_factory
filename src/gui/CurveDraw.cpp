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

#include <QLayout>
#include <QSpacerItem>
#include <QLabel>


#include <qwt/qwt_scale_engine.h>
#include <qwt/qwt_picker_machine.h>

#include "CurveDraw.hpp"

CurveDraw::CurveDraw(QWidget *parent, uint32_t tab_size, uint8_t gtype /*= WAVE*/)
	:QWidget(parent), size(tab_size){

    //parent->resize(800, 500);
    //QSize wsize = parent->size();
    //this->resize(wsize.width() - 10, wsize.height() - 10);
    this->resize(800, 500);
    lay = new QHBoxLayout();
    vlay = new QVBoxLayout();

    /*Plot*/
    graph = new QwtPlot();
    lay->addWidget(graph);

    /*send button*/
    QSpacerItem *space = new QSpacerItem(50, 50);
    vlay->addItem(space);

    send_btn = new QPushButton("\n\nSend Data\n\n");
    vlay->addWidget(send_btn);
    connect(send_btn, SIGNAL(clicked()), this, SLOT(send_graph()));

    QSpacerItem *space2 = new QSpacerItem(50, 50);
    vlay->addItem(space2);

}

void CurveDraw::enable_vol_knob(void){

    wp->setStateMachine(new QwtPickerClickPointMachine());
    wp->setTrackerMode(QwtPicker::ActiveOnly);
    wp->setRubberBand(QwtPicker::CrossRubberBand);

    /*Volume knob common to all*/
    QLabel* vol_label = new QLabel("Volume");
    vol_label->setAlignment(Qt::AlignCenter);
    vlay->addWidget(vol_label);
    vol_btn = new QwtKnob();
    vol_btn->setRange(0.0, 1.0);
    vol_btn->setValue(0.8);
    connect(vol_btn, SIGNAL(valueChanged(double)), this, SLOT(send_vol(double)));
    vlay->addWidget(vol_btn);

    /* draw*/
    setLayout(lay);
    lay->addLayout(vlay);
    graph->replot();

}

CurveDraw::~CurveDraw()
{
    delete graph;
    delete wp;
}

void CurveDraw::draw()
{
    wp->draw();
    graph->replot();
}

void CurveDraw::mousePressEvent(QMouseEvent *ev){
    ev = ev;
    draw();
}

void CurveDraw::mouseMoveEvent(QMouseEvent *ev){
    ev = ev;
    draw();
}

void CurveDraw::send_graph(void){

        cout << "#klem test " << endl;;
	//float fgraph[wp->getSize()];
	//for(int i = 0; i < wp->getSize(); i++){
	//	fgraph[i] = wp->getGraph(i);
	//}
	float* fgraph = wp->getGraph();

	//inherit

	delete fgraph;

}



void CurveDraw::setTitle(QString ttl){
	graph->setTitle(ttl);
}

/*
 * TimeCurveDraw
 */
TimeCurveDraw::TimeCurveDraw(QWidget *parent, uint32_t tab_size)
	:CurveDraw(parent, tab_size){

	graph->enableAxis(QwtPlot::xBottom, false);
	graph->setAxisScale(QwtPlot::yLeft, -1, 1);
	graph->setAxisTitle(QwtPlot::yLeft, "Amplitude");

	wp = new TimePicker(graph->canvas(), graph, size);
	wp->setBound(-1, 1);
}


/*
 * FreqCurveDraw
 */
FreqCurveDraw::FreqCurveDraw(QWidget *parent, uint32_t tab_size)
	:CurveDraw(parent, tab_size){

	graph->setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine());
	graph->setAxisScale(QwtPlot::xBottom, 100, 20000);
	graph->setAxisScale(QwtPlot::yLeft, 0, 1);
	graph->setAxisTitle(QwtPlot::xBottom, "Frequency");
	graph->setAxisTitle(QwtPlot::yLeft, "Amplitude");

	wp = new FreqPicker(graph->canvas(), graph, size, FILTER);
	wp->setBound(0, 1);

}

void FreqCurveDraw::enable_freq_shift_knob(void){

    QLabel* freq_label = new QLabel("Freq shift");
    freq_label->setAlignment(Qt::AlignCenter);
    vlay->addWidget(freq_label);
    freq_btn = new QwtKnob();
    freq_btn->setRange(-1.0, 1.0);
    freq_btn->setValue(0.0);
	connect(freq_btn, SIGNAL(valueChanged(double)), this, SLOT(send_freq(double)));
    vlay->addWidget(freq_btn);

}



