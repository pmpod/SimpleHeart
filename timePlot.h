#pragma once
#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include <QObject>
#include <Qwidget>
#include "defaultPlot.h"
const int PLOT_SIZE = 401;

class TimePlot : public DefaultPlot
{
   Q_OBJECT

public:
	TimePlot(QWidget* = NULL);

public slots:
    void addValue( double value, int id_electrode );
    void addValue( double valueTime, double ValuePotential, int id_electrode );
	void addValue_e1(double valueTime, double value );
	void addValue_e2(double valueTime, double value );
	void addValue_e3(double valueTime, double value );
	void paneLeft();
	void paneRight();
	void paneUp();
	void paneDown();
	void clear();
	void zoomInX();
	void zoomOutX();
	void zoomInY();
	void zoomOutY();

	void autoscale();
	//void clear_e1();
	//void clear_e2();
	//void clear_e3();
private:

    double d_x[PLOT_SIZE]; 
    double d_y[PLOT_SIZE]; 

    int d_interval; // scale timer in ms
    int d_timerId;

	int size_x;

	double scale_Yu;
	double scale_Yd;
	
	double curvalue_1;
	double curvalue_2;
	double curvalue_3;

	double derivative_1_1;
	double derivative_1_2;
	double derivative_2_1;
	double derivative_2_2;
	double derivative_3_1;
	double derivative_3_2;

	double lastTime;
	double increment;
};





















//#pragma once
//#include <qwt_plot.h>
//#include <qwt_painter.h>
//#include <qwt_plot_canvas.h>
//#include <qwt_plot_marker.h>
//#include <qwt_plot_curve.h>
//#include <qwt_scale_widget.h>
//#include <qwt_legend.h>
//#include <qwt_scale_draw.h>
//#include <qwt_math.h>
//#include <QObject>
//#include <Qwidget>
//const int PLOT_SIZE = 201;
//
//class TimePlot : public QwtPlot
//{
//   Q_OBJECT
//
//public:
//	TimePlot(QWidget* = NULL);
//
//public slots:
//    void setScaleInterval(double interval);
//    virtual void addValue(double value);
//
//private:
//    void alignScales();
//
//    double d_x[PLOT_SIZE]; 
//    double d_y[PLOT_SIZE]; 
//
//    int d_interval; // scale timer in ms
//    int d_timerId;
//};
