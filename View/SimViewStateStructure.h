#pragma once
#include "View\SimViewState.h"
#include "Model\Oscillator.h"
#include <map>
class SimViewStateStructure: public SimViewState
{
public:
	~SimViewStateStructure();
	SimViewStateStructure(glAtrium* view);

	static SimViewState* Instance(glAtrium*);
	void handleMouseLeftPress(glAtrium*, QMouseEvent *event);
	void handleMouseRightPress(glAtrium*, QMouseEvent *event);
	void handleMouseRelease(glAtrium*, QMouseEvent *event);
	void handleMousewheel(glAtrium*, QWheelEvent *event);
	void handleMouseMove(glAtrium*, QMouseEvent *event);

protected:
private:
	void paintStructureInRadius(Oscillator* src, Oscillator* osc, const double radius, CELL_TYPE type);
	static SimViewStateStructure* _instance;


	//painting handling
	std::map < int, bool > m_isPaintedMap;
	CELL_TYPE m_currentDrawType;
	CELL_TYPE m_paintType;
	CELL_TYPE m_clearType;

	//cursor handling
	double cursorRadius;
	QPixmap _cursorPixmap;
	QCursor paintCursor(glAtrium*, float radius);

	glAtrium* _view;
};

