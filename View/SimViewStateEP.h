#pragma once
#include "View\SimViewState.h"


class SimViewStateEP: public SimViewState
{
public:
	~SimViewStateEP();
	SimViewStateEP();

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
	static SimViewStateEP* _instance;
	short _dataDisplayMode;
};

