#include <qwt_math.h>
#include <qwt_symbol.h>

#include <qwt_plot_canvas.h>
#include <qwt_text.h>
#include "rrPlot.h"

//TODO manage fonts
QFont z_axisFont("Times", 8);
QFont z_legendsFont("Times", 8);
QFont z_titleFont("Times", 8);

rrPlot::rrPlot(QWidget *parent): 
		DefaultPlot(parent)
{

    // Disable polygon clipping
//    QwtPainter::setDeviceClipping(false);
    // We don't need the cache here
//    canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
//    canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);

    alignScales();



  // axes
	maxRangeX = 100;
	minRangeX = 0;
	currentText.setText("RR number [index]");
	currentText.setFont(z_axisFont);
    setAxisTitle(QwtPlot::xBottom, currentText);
	currentText.setText("ISI [ms]");
    setAxisTitle(QwtPlot::yLeft, currentText);

	addCurve("nowa krzywa", Qt::white);


	d_curve.back()->setStyle(QwtPlotCurve::Lines);

	scale_Yd=0;
	scale_Yu=1000;


    d_mrk1->setValue(300.0, 0.0);
    d_mrk1->setLineStyle(QwtPlotMarker::HLine);
    d_mrk1->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
    d_mrk1->setLinePen(QPen(QColor(100,150,150), 0, Qt::DashDotLine));


    d_mrk2->setValue(350.0, 0.0);
    d_mrk2->setLineStyle(QwtPlotMarker::HLine);
    d_mrk2->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
    d_mrk2->setLinePen(QPen(QColor(200,150,0), 0, Qt::DashDotLine));


	d_mrk2->setLabel(axisScaleDraw(QwtPlot::xBottom)->label(200));
	d_mrk1->setLabel(axisScaleDraw(QwtPlot::xBottom)->label(200));
}


rrPlot::~rrPlot(void)
{
}

void rrPlot::appendRR(double y)
{

	double x = d_data[0]->count() + 1;
	if (y < 0)
	{
		double z = 0;
		appendData(&x, &z, 1, 0);
	}
	else
	{
		appendData(&x, &y, 1, 0);
	}

	//przeskalowywanie
	if( maxRangeX <= x )
	{
		maxRangeX += 15;
		minRangeX += 15;
		setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);
	}
}

void rrPlot::clear()
{
	for(unsigned int i = 0; i < d_curve.size(); i++)
	{
		d_curve[i]->detach();
		//d_curve[i]->clear();
	}
    d_curve.clear();
	for(unsigned int i = 0; i < d_data.size(); i++)
	{
		d_data[i]->clear();
	}
	d_data.clear();
	addCurve("nowa krzywa", Qt::black);


	maxRangeX = 30;
	minRangeX = 0;
	setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);

    replot();
}

void rrPlot::paneLeft()
{
		maxRangeX -= 3;
		minRangeX -= 3;
		setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);

}
void rrPlot::paneRight()
{
	double x = d_data[0]->count() + 1;

	if( minRangeX <= x )
	{
		maxRangeX += 3;
		minRangeX += 3;
		setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);
	}
}
void rrPlot::setXmin(double value)
{
	minRangeX = value;
	setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);
}
void rrPlot::setXmax(double value)
{
	maxRangeX = value;
	setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);
}
void rrPlot::setYmin(double value)
{
	scale_Yd=value;
	setAxisScale( QwtPlot::yLeft , scale_Yd, scale_Yu);
}
void rrPlot::setYmax(double value)
{
	scale_Yu=value;
	setAxisScale( QwtPlot::yLeft , scale_Yd, scale_Yu);
}
void rrPlot::zoomInY()
{
	scale_Yu-=25;
	if(scale_Yu<=25) scale_Yu=25;
	setAxisScale( QwtPlot::yLeft , scale_Yd, scale_Yu);
}
void rrPlot::zoomOutY()
{
	scale_Yu+=25;
	setAxisScale( QwtPlot::yLeft , scale_Yd, scale_Yu);
}
void rrPlot::zoomInX()
{
	maxRangeX -= 15;
	setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);
}
void rrPlot::zoomOutX()
{
	maxRangeX += 15;
	setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);

}
void rrPlot::autoscale()
{
	setAxisAutoScale( QwtPlot::yLeft ) ;
	scale_Yu = 1000;
	scale_Yd = 0;
}
void rrPlot::paneUp()
{
	scale_Yd-=(scale_Yu+scale_Yd)/10;
	scale_Yu+=(scale_Yu+scale_Yd)/10;
	setAxisScale( QwtPlot::yLeft , -scale_Yd, scale_Yu);
}
void rrPlot::paneDown()
{
	scale_Yd+=(scale_Yu+scale_Yd)/10;
	scale_Yu-=(scale_Yu+scale_Yd)/10;
	setAxisScale( QwtPlot::yLeft , -scale_Yd, scale_Yu);
}

void  rrPlot::mousePressEvent(QMouseEvent *event)
{
	QwtText lable_value;
	switch(event->button())
	{
		case Qt::LeftButton:
					d_mrk1->setValue(0.0, invTransform(QwtPlot::yLeft, event->pos().y()));
					lable_value = axisScaleDraw(QwtPlot::xBottom)->label(static_cast<double>(static_cast<int>(d_mrk2->yValue()-d_mrk1->yValue())));
					d_mrk2->setLabel(lable_value);
					d_mrk1->setLabel(lable_value);
					break;
		case Qt::RightButton:
					d_mrk2->setValue(0.0, invTransform(QwtPlot::yLeft, event->pos().y()));
					lable_value = axisScaleDraw(QwtPlot::xBottom)->label(static_cast<double>(static_cast<int>(d_mrk2->yValue()-d_mrk1->yValue())));
					d_mrk1->setLabel(lable_value);
					d_mrk2->setLabel(lable_value);
					break;
		default:
	break;
	}
}