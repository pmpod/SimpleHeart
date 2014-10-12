#pragma once
#include "CurveData.h"
#include "defaultPlot.h"



class rrPlot: public DefaultPlot
{
    Q_OBJECT

public:
    rrPlot(QWidget *parent);
	virtual ~rrPlot();

	double scale_Yd;
	double scale_Yu;
public slots:
 
    void appendRR(double y);
	void paneLeft();
	void paneRight();
	void clear();
	void paneUp();
	void paneDown();
	void zoomInX();
	void zoomOutX();
	void zoomInY();
	void zoomOutY();
	void setXmin(double value);
	void setXmax(double value);
	void setYmin(double value);
	void setYmax(double value);
	void autoscale();
	void mousePressEvent ( QMouseEvent *event );
};
