#pragma once
#include "View\SimViewState.h"


class SimViewStateView: public SimViewState
{
public:
	~SimViewStateView();
	SimViewStateView();

	static SimViewState* Instance();
	void setMode(const int mode); 
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

	void processActivationTimes(glAtrium* view, int oscillatorID);
protected:
private:
	static SimViewStateView* _instance;
	short _mode;

	//%activation time visualization
	double _derivativeTreshold;
	double _minRefractoryTreshold;
	GLfloat vmin;
	GLfloat vmax;
	GLfloat previous_vmin;
	GLfloat previous_vmax;
	GLfloat helper2;
};

