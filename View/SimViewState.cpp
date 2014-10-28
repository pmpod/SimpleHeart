#include "View\SimViewState.h"
#include "glAtrium.h"


SimViewState::SimViewState()
{
}


SimViewState::~SimViewState()
{
}

void SimViewState::ChangeState(glAtrium* view, SimViewState* s)
{
	view->ChangeState(s);
}
const double SimViewState::getRadius(){ return cursorRadius; }