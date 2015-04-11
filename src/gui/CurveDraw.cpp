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

    /* Specific init */
    /*
    type = gtype;

    if(type == WAVE){

        graph->setTitle("Draw your wave");
        graph->enableAxis(QwtPlot::xBottom, false);
        graph->setAxisScale(QwtPlot::yLeft, -1, 1);
        graph->setAxisTitle(QwtPlot::yLeft, "Amplitude");

        wp = new TimePicker(graph->canvas(), graph, tab_size);
        wp->setBound(-1, 1);

        port = 2323;

      }else if(type == SPECTRUM){

        graph->setTitle("Draw your spectrum");
        graph->setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine());
        graph->setAxisScale(QwtPlot::xBottom, 100, 20000);
        graph->setAxisScale(QwtPlot::yLeft, 0, 1);
        graph->setAxisTitle(QwtPlot::xBottom, "Frequency");
        graph->setAxisTitle(QwtPlot::yLeft, "Amplitude");

        wp = new FreqPicker(graph->canvas(), graph, tab_size, type);
        wp->setBound(0, 1);

        QLabel* freq_label = new QLabel("Freq shift");
        freq_label->setAlignment(Qt::AlignCenter);
        vlay->addWidget(freq_label);
        freq_btn = new QwtKnob();
        freq_btn->setRange(-1.0, 1.0);
        freq_btn->setValue(0.0);
    	connect(freq_btn, SIGNAL(valueChanged(double)), this, SLOT(send_freq(double)));
        vlay->addWidget(freq_btn);

        port = 2324;

      }else if(type == FILTER){

        graph->setTitle("Draw your filter spectrum");
        graph->setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine());
        graph->setAxisScale(QwtPlot::xBottom, 100, 20000);
        graph->setAxisScale(QwtPlot::yLeft, 0, 1);
        graph->setAxisTitle(QwtPlot::xBottom, "Frequency");
        graph->setAxisTitle(QwtPlot::yLeft, "Amplitude");

        wp = new FreqPicker(graph->canvas(), graph, tab_size, type);
        wp->setBound(0, 1);

        QLabel* freq_label = new QLabel("Freq shift");
        freq_label->setAlignment(Qt::AlignCenter);
        vlay->addWidget(freq_label);
        freq_btn = new QwtKnob();
        freq_btn->setRange(-1.0, 1.0);
        freq_btn->setValue(0.0);
    	connect(freq_btn, SIGNAL(valueChanged(double)), this, SLOT(send_freq(double)));
        vlay->addWidget(freq_btn);

        port = 2325;

      }else if(type == CFILTER){

        graph->setTitle("Draw your filter spectrum");
        graph->setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine());
        graph->setAxisScale(QwtPlot::xBottom, 100, 20000);
        graph->setAxisScale(QwtPlot::yLeft, 0, 1);
        graph->setAxisTitle(QwtPlot::xBottom, "Frequency");
        graph->setAxisTitle(QwtPlot::yLeft, "Amplitude");

        wp = new FreqPicker(graph->canvas(), graph, tab_size, type);
        wp->setBound(0, 1);

        port = 2326;
      }
*/
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

#ifdef OSC
        cout << "#klem test osc size :" << wp->getSize() << endl;

        lo_blob data = lo_blob_new(wp->getSize()*sizeof(float), fgraph);
        if(lo_send(osc_addr, "/wave/table", "b", data) == -1) {
                printf("OSC error %d: %s\n", lo_address_errno(osc_addr), lo_address_errstr(osc_addr));
        }else{
                printf("OSC message send \n");
        }

#endif

#ifdef LV2_GUI
        //this->lv2_write(lv2_ctrl, 2, wp->getSize(), 0, fgraph);
        //this->lv2_write(lv2_ctrl, 2, wp->getSize(), atom_vector_urid, fgraph);
//      toto = new float;
//      *toto = 1;
//      this->lv2_write(lv2_ctrl, 2, sizeof(float), 0, toto);   
#endif

	delete fgraph;

}

/*
void CurveDraw::send_vol(double value){
#ifdef LV2_GUI
	lv2_send_data(3, value);
#endif

#ifdef OSC
	osc_send_data(3, value);
#endif
}

void CurveDraw::send_freq(double value){
#ifdef LV2_GUI
	lv2_send_data(2, value);
#endif

#ifdef OSC
	osc_send_data(2, value);
#endif
}
*/

#ifdef LV2_GUI
void CurveDraw::set_lv2_ctrl(LV2UI_Controller lc){
        //if(lc != NULL){
	lv2_ctrl = lc;
        //}
}

void CurveDraw::set_lv2_write_fn(LV2UI_Write_Function lw){
        //if(lw != NULL){
	lv2_write = lw;
        //}
}

void CurveDraw::lv2_send_data(uint32_t index, double val){

         float value = val;
         if((lv2_write != NULL)&&(lv2_ctrl != NULL)){

                lv2_write(lv2_ctrl, index, sizeof(float), 0, &value);
         }
}

#endif

#ifdef OSC
void CurveDraw::osc_send_data(uint32_t index, double val){

	string path;
	float value = val;

	if(index == 2) path = "/wave/freq_shift";
	else if(index == 3) path = "/wave/volume";

	if(lo_send(osc_addr, path.c_str(), "f", value) == -1) {
			printf("OSC error %d: %s\n", lo_address_errno(osc_addr), lo_address_errstr(osc_addr));
    }else{
            printf("OSC message send \n");
    }
}

#endif

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



