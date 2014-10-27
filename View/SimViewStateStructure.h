#pragma once
#include "View\SimViewState.h"
#include "Model\Oscillator.h"

class SimViewStateStructure: public SimViewState
{
public:
	~SimViewStateStructure();
	SimViewStateStructure();

	static SimViewState* Instance();
	void handleMouseLeftPress(glAtrium*, QMouseEvent *event);
	void handleMouseRightPress(glAtrium*, QMouseEvent *event);
	void handleMouseRelease(glAtrium*, QMouseEvent *event);
	void handleMousewheel(glAtrium*, QWheelEvent *event);
	void handleMouseMove(glAtrium*, QMouseEvent *event);

protected:
private:
	void paintStructureInRadius(Oscillator* src, Oscillator* osc, const double radius, CELL_TYPE type);
	static SimViewStateStructure* _instance;
	std::vector<Oscillator*> m_painted;

	QPixmap _cursorPixmap;

	QCursor paintCursor(glAtrium*, float radius);

};

