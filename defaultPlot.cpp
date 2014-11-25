#include <qwt_math.h>
#include <qwt_symbol.h>
#include <qwt_picker_machine.h>


#include <qwt_plot_canvas.h>
#include <qwt_text.h>
#include "defaultPlot.h"

QFont g_axisFont("Times", 7);
QFont g_legendsFont("Times", 7);
QFont g_titleFont("Times", 7);


using namespace std;

DefaultPlot::DefaultPlot(QWidget *parent): 
		QwtPlot(parent), maxRangeX(100), minRangeX(0), maxRangeY(50), minRangeY(0)
{
 // Wygl¹d ogólny plot-u

	setAutoReplot(false);
	
	

	setFrameStyle(QFrame::NoFrame);
    setLineWidth(0);
    setCanvasLineWidth(2);

    plotLayout()->setAlignCanvasToScales(true);
    setCanvasBackground(QColor(255, 255, 255)); // nice white

	alignScales();
 //siatka:
    grid = new QwtPlotGrid;
 
    grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
    grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid->attach(this);
	

 // markery
    d_mrk1 = new QwtPlotMarker();
    d_mrk1->setValue(100.0, 0.0);
    d_mrk1->setLineStyle(QwtPlotMarker::VLine);
    d_mrk1->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
    d_mrk1->setLinePen(QPen(QColor(100,150,150), 0, Qt::DashDotLine));
    d_mrk1->attach(this);

    d_mrk2 = new QwtPlotMarker();
    d_mrk2->setLineStyle(QwtPlotMarker::VLine);
    d_mrk2->setValue(300.0, 0.0);
    d_mrk2->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
    d_mrk2->setLinePen(QPen(QColor(200,150,0), 0, Qt::DashDotLine));
    d_mrk2->attach(this);


	d_mrk2->setLabel(axisScaleDraw(QwtPlot::xBottom)->label(200));
	d_mrk1->setLabel(axisScaleDraw(QwtPlot::xBottom)->label(200));


    QwtPlotPicker *d_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                                 QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                                 canvas());
	
    d_picker->setStateMachine(new QwtPickerPolygonMachine());
    d_picker->setRubberBandPen(QColor(Qt::yellow));
    d_picker->setRubberBand(QwtPicker::CrossRubberBand);
    d_picker->setTrackerPen(QColor(Qt::white));
    //connect(d_picker,SIGNAL(moved(QPoint)),this,SLOT(moveRightMark(QPoint)));

 // osie
    setAxisMaxMajor(QwtPlot::xBottom, 8);
    setAxisMaxMinor(QwtPlot::xBottom, 8);
	setAxisMaxMajor(QwtPlot::yLeft, 8);
    setAxisMaxMinor(QwtPlot::yLeft, 8);


	setAxisFont(QwtPlot::xBottom, g_axisFont);
	setAxisFont(QwtPlot::yLeft, g_axisFont);


	d_picker = new QwtPlotPicker((QwtPlot::xBottom, QwtPlot::yLeft, 
								 QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, 
								 this->canvas()));

    d_picker->setRubberBandPen(QColor(Qt::red));
    d_picker->setRubberBand(QwtPicker::CrossRubberBand);
    d_picker->setTrackerPen(QColor(Qt::black));

    setAutoReplot(true);
}

 
void DefaultPlot::setScaleMinX( double min_RangeX)
{
	minRangeX = min_RangeX;
    setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);
}
void DefaultPlot::setScaleMaxX( double max_RangeX )
{
	maxRangeX = max_RangeX;
    setAxisScale(QwtPlot::xBottom, minRangeX, maxRangeX);
}
void DefaultPlot::setScaleMinY( double min_RangeY )
{
	minRangeY = min_RangeY;
	setAxisScale(QwtPlot::yLeft, minRangeY, maxRangeY);
}
void DefaultPlot::setScaleMaxY( double max_RangeY )
{
	maxRangeY = max_RangeY;
	setAxisScale(QwtPlot::yLeft, minRangeY, maxRangeY);
}
DefaultPlot::~DefaultPlot(void)
{
}

void DefaultPlot::alignScales()
{
    canvas()->setFrameStyle(QFrame::Box | QFrame::Plain );
    canvas()->setLineWidth(1);

    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = (QwtScaleWidget *)axisWidget(i);
        if ( scaleWidget )
            scaleWidget->setMargin(0);

        QwtScaleDraw *scaleDraw = (QwtScaleDraw *)axisScaleDraw(i);
        if ( scaleDraw )
            scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }
}
unsigned int DefaultPlot::addCurve(std::string name, QColor color)
{
	d_data.push_back(new CurveData);
	d_curve.push_back(new QwtPlotCurve("new"));
	d_curve.back()->setStyle(QwtPlotCurve::Lines);
	d_curve.back()->setPen(color);
    //d_curve.back()->setPaintAttribute(QwtPlotCurve::);
    //d_curve.back()->setSymbol(QwtSymbol(QwtSymbol::Diamond, QBrush(Qt::black), QPen(Qt::black), QSize(7, 7)) );
    d_curve.back()->attach(this);

	return ( d_curve.size() - 1 );
}



void DefaultPlot::appendData(double x, double y, int curve_id)
{
	appendData(&x, &y, 1, curve_id);
}

void DefaultPlot::appendData(double *x, double *y, int size, int curve_id)
{
    if ( d_data.size() == 0 )
	{
		addCurve("new RRs", Qt::black);
	}

    d_data[curve_id]->append(x, y, size);
    d_curve[curve_id]->setRawSamples( d_data[curve_id]->x(), d_data[curve_id]->y(), d_data[curve_id]->count() );

  //  const bool cacheMode = canvas()->testPaintAttribute(QwtPlotCanvas::PaintCached);
  //  canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
    d_curve[curve_id]->plot();//(d_curve[curve_id]->dataSize() - size, d_curve[curve_id]->dataSize() - 1);
  //  canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, cacheMode);

}

void DefaultPlot::removeData()
{
    d_curve.empty();
	d_data.empty();
    replot();
}
void DefaultPlot::clear()
{
    d_curve.clear();
	d_data.clear();
    replot();
}


void  DefaultPlot::moveLeftMark(QPoint pos)
{
}
void  DefaultPlot::moveRightMark(QPoint pos)
{
    QwtText lable_value = axisScaleDraw(QwtPlot::xBottom)->label(invTransform(QwtPlot::xBottom,pos.x()));
    double xTop = invTransform(QwtPlot::xTop, pos.x());
    double yLeft = invTransform(QwtPlot::yLeft, pos.y());
    d_mrk1->setValue(xTop,yLeft);
    d_mrk1->setLabel(lable_value);
    replot();
}
void  DefaultPlot::mousePressEvent(QMouseEvent *event)
{
	QwtText lable_value;
	switch(event->button())
	{
		case Qt::LeftButton:
					d_mrk1->setValue(invTransform(QwtPlot::xBottom, event->pos().x())-59, 0.0);
					lable_value = axisScaleDraw(QwtPlot::xBottom)->label(d_mrk2->xValue()-d_mrk1->xValue());
					d_mrk2->setLabel(lable_value);
					d_mrk1->setLabel(lable_value);
					break;
		case Qt::RightButton:
					d_mrk2->setValue(invTransform(QwtPlot::xBottom, event->pos().x())-59, 0.0);
					lable_value = axisScaleDraw(QwtPlot::xBottom)->label(d_mrk2->xValue()-d_mrk1->xValue());
					d_mrk1->setLabel(lable_value);
					d_mrk2->setLabel(lable_value);
					break;
		default:
	break;
	}

}
double DefaultPlot::getLastValue()
{
	double val = (d_data[0]->y())[d_data[0]->count()];
	return val;
}