#include "View\SimViewState.h"
#include "glAtrium.h"


SimViewState::SimViewState()
{
	_scale = 0.1;

}


SimViewState::~SimViewState()
{
}

void SimViewState::ChangeState(glAtrium* view, SimViewState* s)
{
	view->ChangeState(s);
}
const double SimViewState::getRadius(){ return cursorRadius; }
void  SimViewState::setPalette(const DISP_PALETTE pal)
{
	_palette = pal;
	switch (pal)
	{
	case DP_GRAY:
		colorMap = &grayMap;
		break;
	case DP_HOTTOCOLD:
		colorMap = &hotToColdMap;
		break;
	case DP_HOT:
		colorMap = &hotMap;
		break;
	case DP_COLD:
		colorMap = &coldMap;
		break;
	}
}