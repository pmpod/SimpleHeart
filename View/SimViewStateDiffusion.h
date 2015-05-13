#pragma once
#include "View\SimViewState.h"
#include "Model\Oscillator.h"
#include <map>

enum paintMode
{
	PAINT_DIFF,
	PAINT_ERP
};
class SimViewStateDiffusion: public SimViewState
{
public:
	~SimViewStateDiffusion();
	SimViewStateDiffusion();

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

protected:

private:
	static SimViewStateDiffusion* _instance;
	void paintDiffusionInRadius(Oscillator* src, Oscillator* osc, double value);

	//painting handling
	std::map < int, bool > m_isPaintedMap;


	double _diffPaintingStrengthValue;
	double _ERPpaintingStrengthValue;
	double _ExcitabilityPaintingStrengthValue;
	double _gaussSigma;
	BRUSH_OUTLINE _outline;
	short _whatToPaint;
	
};

