#pragma once
#include <QGLWidget>
#include <QtOpenGL>
#include <QtGui>
#include "GLfunc.h"

enum DISP_PALETTE
{
	DP_GRAY,
	DP_HOT,
	DP_HOTTOCOLD,
	DP_COLD
};

class glAtrium;
class SimViewState
{
public:
	SimViewState();
	~SimViewState();

	virtual void handleMouseLeftPress(glAtrium* view, QMouseEvent *event) = 0;
	virtual void handleMouseRightPress(glAtrium* view, QMouseEvent *event) = 0;
	virtual void handleMouseRelease(glAtrium* view, QMouseEvent *event) = 0;
	virtual void handleMousewheel(glAtrium* view, QWheelEvent *event) = 0;
	virtual void handleMouseMove(glAtrium* view, QMouseEvent *event) = 0;


	virtual void paintModel(glAtrium* view) = 0;
	virtual void paintCursor(glAtrium* view) = 0;
	virtual void paintLegend(glAtrium*) = 0;
	virtual void prepareLegend(glAtrium*) = 0;

	virtual void setDisplayMode(const int mode) = 0;
	void setPalette(const DISP_PALETTE pal);

	const double getRadius();

protected:
	void(*colorMap)(GLfloat &v, const GLfloat &vmin, const GLfloat &vmax, GLfloat &rr, GLfloat &gg, GLfloat &bb);
	void ChangeState(glAtrium* view, SimViewState* s);
	double cursorRadius;
	QPixmap _cursorPixmap;
	float _scale;

private:
	DISP_PALETTE _palette;
};

