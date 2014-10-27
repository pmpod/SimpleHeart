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

protected:
	void ChangeState(glAtrium* view, SimViewState* s);
};

