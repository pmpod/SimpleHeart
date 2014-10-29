#pragma once
#include <QGLWidget>
#include <QtOpenGL>
#include <QtGui>

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
	virtual void paintCursor(glAtrium* view) = 0;
	virtual void paintModel(glAtrium* view) = 0; 
	virtual void setDisplayMode(const int mode) = 0;
	const double getRadius();

protected:
	void ChangeState(glAtrium* view, SimViewState* s);
	double cursorRadius;
	QPixmap _cursorPixmap;

private:

};

