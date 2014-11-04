#pragma once
#include "View\SimViewState.h"


class SimViewStateView: public SimViewState
{
public:
	~SimViewStateView();
	SimViewStateView();

	static SimViewState* Instance();
	void setDisplayMode(const int mode); 
	void setOutlineStyle(const  BRUSH_OUTLINE outline);
	void handleMouseLeftPress(glAtrium*, QMouseEvent *event);
	void handleMouseRightPress(glAtrium*, QMouseEvent *event);
	void handleMouseRelease(glAtrium*, QMouseEvent *event);
	void handleMousewheel(glAtrium*, QWheelEvent *event);
	void handleMouseMove(glAtrium*, QMouseEvent *event);
	void paintLegend(glAtrium*);
	void paintCursor(glAtrium*);
	void paintModel(glAtrium*);
	void prepareLegend(glAtrium*);
protected:
private:
	static SimViewStateView* _instance;
	short _dataDisplayMode;
};

