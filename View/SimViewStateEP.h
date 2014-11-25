#pragma once
#include "View\SimViewState.h"
#include "Model\Oscillator.h"

class SimViewStateEP: public SimViewState
{
public:
	~SimViewStateEP();
	SimViewStateEP();

	static SimViewState* Instance();
	void setOutlineStyle(const  BRUSH_OUTLINE outline);
	void handleMouseLeftPress(glAtrium*, QMouseEvent *event);
	void handleMouseRightPress(glAtrium*, QMouseEvent *event);
	void handleMouseRelease(glAtrium*, QMouseEvent *event);
	void handleMousewheel(glAtrium*, QWheelEvent *event);
	void handleMouseMove(glAtrium*, QMouseEvent *event);
	void paintLegend(glAtrium*);
	void paintCursor(glAtrium*);
	void prepareLegend(glAtrium*);
protected:
	int findElectrode(glAtrium* view, Oscillator* src, Oscillator* osc);
private:
	static SimViewStateEP* _instance;
	short _probeOnTheMove;
	std::map < int, bool > m_isSearchedMap;
};

