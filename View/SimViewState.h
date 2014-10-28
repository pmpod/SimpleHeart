#pragma once
#include <QGLWidget>

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
	virtual void paintCursor(glAtrium*, float radius) = 0;
	const double getRadius();

protected:
	void ChangeState(glAtrium* view, SimViewState* s);
	double cursorRadius;
	QPixmap _cursorPixmap;
	glAtrium* _view;

private:

};

