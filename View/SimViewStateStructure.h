#pragma once
#include "View\SimViewState.h"
#include "Model\Oscillator.h"
#include <map>
class SimViewStateStructure: public SimViewState
{
public:
	~SimViewStateStructure();
	SimViewStateStructure();

	static SimViewState* Instance();
	void setDisplayMode(const int mode);
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
	static SimViewStateStructure* _instance;
	void paintStructureInRadius(Oscillator* src, Oscillator* osc, const double radius, CELL_TYPE type);

	//painting handling
	std::map < int, bool > m_isPaintedMap;

	std::map < CELL_TYPE, GLfloat > m_structureColorMap;
	CELL_TYPE m_currentDrawType;
	CELL_TYPE m_paintType;
	CELL_TYPE m_clearType;

	//cursor handling
};

