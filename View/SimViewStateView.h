#pragma once
#include "View\SimViewState.h"


class SimViewStateView: public SimViewState
{
public:
	~SimViewStateView();
	SimViewStateView(glAtrium* view);

	static SimViewState* Instance(glAtrium* view);
	void handleMouseLeftPress(glAtrium*, QMouseEvent *event);
	void handleMouseRightPress(glAtrium*, QMouseEvent *event);
	void handleMouseRelease(glAtrium*, QMouseEvent *event);
	void handleMousewheel(glAtrium*, QWheelEvent *event);
	void handleMouseMove(glAtrium*, QMouseEvent *event);
	void paintCursor(glAtrium*, float radius);

protected:
private:
	static SimViewStateView* _instance;
};

