#include "timePlot.h"

QFont q_axisFont("Arial", 8);
QFont q_noaxisFont("Arial", 0);

TimePlot::TimePlot(QWidget *parent):
    DefaultPlot(parent),
    d_interval(0),
    d_timerId(-1)
{
    // Disable polygon clipping
    //QwtPainter::setDeviceClipping(false);

    // Assign a title
	currentText.setFont(q_axisFont);
    // Axis 
	currentText.setText("Time [s]");
    setAxisScale(QwtPlot::xBottom, 0, 99);
	currentText.setText("Potential");
    setAxisTitle(QwtPlot::yLeft, currentText);
    setAxisScale(QwtPlot::yLeft, -3.5, 3.5);
   
	addCurve("e1", Qt::yellow);
    addCurve("e2", Qt::red);
    addCurve("e3", Qt::green);
    //setScaleInterval(0.0);
	maxRangeX = 1999;
	minRangeX = 0;
	setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);

	replot();

	size_x = 0;
	scale_Yu = 3.5;
	scale_Yd = 3.5;
	increment = 1750;
	curvalue_1=0;
	curvalue_2=0;
	curvalue_3=0;
	lastTime=0;
	setAxisScale( QwtPlot::yLeft , -1.5, 1.5);
}
void TimePlot::zoomInY()
{
	scale_Yd-=scale_Yd/6;
	scale_Yu-=scale_Yu/6;
	setAxisScale( QwtPlot::yLeft , -scale_Yd, scale_Yu);
}
void TimePlot::zoomOutY()
{
	scale_Yd+=scale_Yd/6;
	scale_Yu+=scale_Yu/6;
	setAxisScale( QwtPlot::yLeft , -scale_Yd, scale_Yu);
}
void TimePlot::zoomInX()
{
	maxRangeX -= 500;
	if (maxRangeX <= minRangeX) maxRangeX = minRangeX + 500;
	increment -=500;
	if (increment <= 250) maxRangeX = 250;
	setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);
}
void TimePlot::zoomOutX()
{
	maxRangeX += 500;
	increment += 500;
	setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);

}
void TimePlot::autoscale()
{
	setAxisAutoScale  ( QwtPlot::yLeft ) ;
	scale_Yu = 3.5;
	scale_Yd = 3.5;
}
void TimePlot::paneDown()
{
	scale_Yd+=(scale_Yu+scale_Yd)/10;
	scale_Yu-=(scale_Yu+scale_Yd)/10;;
	setAxisScale( QwtPlot::yLeft , -scale_Yd, scale_Yu);
}
void TimePlot::paneUp()
{
	scale_Yd-=(scale_Yu+scale_Yd)/10;;
	scale_Yu+=(scale_Yu+scale_Yd)/10;;
	setAxisScale( QwtPlot::yLeft , -scale_Yd, scale_Yu);
}
//----------------------------------------------------------------------
void TimePlot::addValue_e1(double valueTime, double value )
{	
	derivative_1_2=derivative_1_1;
	derivative_1_1=curvalue_1;

	//addValue( valueTime, (derivative_1_2-2*value+derivative_1_1)/(valueTime-lastTime), 0 );
	addValue( valueTime, value, 0 );

	curvalue_1=value;
	lastTime=valueTime;
}
void TimePlot::addValue_e2(double valueTime, double value )
{	
	derivative_2_2=derivative_2_1;
	derivative_2_1=curvalue_2;

	//addValue( valueTime, (derivative_2_2-2*value+derivative_2_1)/(valueTime-lastTime),  1 );
	addValue( valueTime, value, 1 );

	curvalue_2=value;
	lastTime=valueTime;
}
void TimePlot::addValue_e3(double valueTime, double value )
{
	derivative_3_2=derivative_3_1;
	derivative_3_1=curvalue_3;

	//addValue( valueTime, (derivative_3_2-2*value+derivative_3_1)/(valueTime-lastTime),  2 );
	addValue( valueTime, value,2 );

	curvalue_3=value;
	lastTime=valueTime;
}
//  Generate new values 

void TimePlot::addValue( double value, int id_electrode )
{

	double x = d_data[0]->count() + 1;

	appendData(&x, &value, 1, id_electrode);
	//przeskalowywanie
	if( maxRangeX <= x )
	{
		maxRangeX += 40;
		minRangeX += 40;
		setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);
	}
}

void TimePlot::paneLeft()
{
		maxRangeX -= 100;
		minRangeX -= 100;
		setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);

}
void TimePlot::paneRight()
{
	double x = d_data[0]->count() + 1;

	if( minRangeX <= x )
	{
		maxRangeX += 100;
		minRangeX += 100;
		setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);
	}


}
void TimePlot::addValue( double valueTime, double ValuePotential, int id_electrode)
{
	size_x++;
	double x = valueTime;

	appendData(&x, &ValuePotential, 1, id_electrode);
	//przeskalowywanie
	if( maxRangeX <= x )
	{
		maxRangeX += increment;
		minRangeX += increment;
		setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);
	}
}

void TimePlot::clear()
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
	addCurve("e1", Qt::yellow);
    addCurve("e2", Qt::red);
    addCurve("e3", Qt::green);
    //setScaleInterval(0.0);
	maxRangeX = 2000;
	minRangeX = 0;
	setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);
    replot();
}












//
//#include "timePlot.h"
//
//
////
////  Initialize main window
////
//TimePlot::TimePlot(QWidget *parent):
//    DefaultPlot(parent),
//    d_interval(0),
//    d_timerId(-1)
//{
//    // Disable polygon clipping
//    QwtPainter::setDeviceClipping(false);
//    // We don't need the cache here
//    //canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
//    //canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);
//
//    alignScales();
//    
//    // Assign a title
//    setTitle("Membrane potential for the SA node cell");
//    insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
//
//    // Insert new curves
//    QwtPlotCurve *cRight = new QwtPlotCurve("Data Moving Right");
//    cRight->attach(this);
//
//    // Set curve styles
//    cRight->setPen(QPen(Qt::red));
//
//    // Attach (don't copy) data. Both curves use the same x array.
//    cRight->setRawData(d_x, d_y, PLOT_SIZE);
//    // Axis 
//    setAxisTitle(QwtPlot::xBottom, "Time/seconds");
//    setAxisScale(QwtPlot::xBottom, 0, 100);
//
//    setAxisTitle(QwtPlot::yLeft, "Values");
//    setAxisScale(QwtPlot::yLeft, -3.5, 3.5);
//    
//    setScaleInterval(0.0);
//}
//
////
////  Set a plain canvas frame and align the scales to it
////
//void TimePlot::alignScales()
//{
//    // The code below shows how to align the scales to
//    // the canvas frame, but is also a good example demonstrating
//    // why the spreaded API needs polishing.
//
//    canvas()->setFrameStyle(QFrame::Box | QFrame::Plain );
//    canvas()->setLineWidth(1);
//
//    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
//    {
//        QwtScaleWidget *scaleWidget = (QwtScaleWidget *)axisWidget(i);
//        if ( scaleWidget )
//            scaleWidget->setMargin(0);
//
//        QwtScaleDraw *scaleDraw = (QwtScaleDraw *)axisScaleDraw(i);
//        if ( scaleDraw )
//            scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
//    }
//}
//
//void TimePlot::setScaleInterval(double interval)
//{
//    d_interval = qRound(interval);
//
//    if ( d_timerId >= 0 )
//    {
//        //killTimer(d_timerId);
//        d_timerId = -1;
//    }
//    //if (d_interval >= 0 )
//       // d_timerId = startTimer(d_interval);
//}
//
////  Generate new values 
//void TimePlot::addValue( double value )
//{
//    // y moves from left to right:
//    // Shift y array right and assign new value to y[0].
//
//    for ( int i = PLOT_SIZE - 1; i > 0; i-- )
//        d_y[i] = d_y[i-1];
//    d_y[0] = value;
//
//    // update the display
//    replot();
//
//}
