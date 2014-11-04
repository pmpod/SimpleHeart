#pragma once
#include <QGLWidget>
#include <QtOpenGL>
#include <QtGui>
#include "GLfunc.h"
#include <boost\math\quaternion.hpp>
#include "Support\Vectors.h"
#include "Support\Matrices.h"
enum DISP_PALETTE
{
	DP_GRAY,
	DP_HOT,
	DP_HOTTOCOLD,
	DP_COLD
};

enum BRUSH_OUTLINE
{
	BRUSH_GAUSS,
	BRUSH_UNI,
	BRUSH_SIN
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
	void paintScale(glAtrium* view);
	virtual void paintLegend(glAtrium*) = 0;
	virtual void prepareLegend(glAtrium*) = 0;

	virtual void setDisplayMode(const int mode) = 0;
	virtual void setOutlineStyle(const  BRUSH_OUTLINE outline) = 0;
	void setPalette(const DISP_PALETTE pal);
	
	const double getRadius();

protected:
	void computeIncremental(Vector3 last, Vector3 next);
	void(*colorMap)(GLfloat &v, const GLfloat &vmin, const GLfloat &vmax, GLfloat &rr, GLfloat &gg, GLfloat &bb);
	void ChangeState(glAtrium* view, SimViewState* s);
	double cursorRadius;
	QPixmap _cursorPixmap;
	float _scale;
	QQuaternion _quatStart;
	QQuaternion _quat;

private:
	DISP_PALETTE _palette;

};

