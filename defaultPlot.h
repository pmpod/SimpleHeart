#pragma once
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_zoomer.h>
#include <qwt_legend.h>
#include "CurveData.h"
#include "heartDefines.h"
#include "atrialParameters.h"
#include <vector>
#include <string>
#include <qwt_scale_widget.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <QtGui>

class QwtPlotPicker;


class DefaultPlot: public QwtPlot
{
    Q_OBJECT
public:
	DefaultPlot(QWidget *parent);
	virtual ~DefaultPlot(void);
    void removeData();

	unsigned int addCurve(std::string name, QColor color);		//returns Curve id
	void removeCurve();
	void setAxisMaxRangeX(double minx, double maxx);
	void setAxisMaxRangeY(double miny, double maxy);

	std::vector<CurveData*> d_data;
	QwtSymbol* s;
	std::vector<QwtPlotCurve*> d_curve;
public slots:
    void appendData(double x, double y, int curve_id);
    void appendData(double *x, double *y, int size, int curve_id);
	void setScaleMinX( double min_RangeX);
	void setScaleMaxX( double max_RangeX );
	void setScaleMinY( double min_RangeY );
	void setScaleMaxY( double max_RangeY );
	void moveLeftMark(QPoint pos);
	void moveRightMark(QPoint pos);

	void clear();
	void mousePressEvent ( QMouseEvent *event );
protected:
	double maxRangeX;
	double minRangeX;
	double maxRangeY;
	double minRangeY;

	QwtText currentText;

	QwtLegend *legend;
	QwtPlotGrid *grid;



    QwtPlotMarker *d_mrk1;
    QwtPlotMarker *d_mrk2;
	QwtPlotPicker *d_picker;
    void alignScales();

	atrialParameters m_defines;
};