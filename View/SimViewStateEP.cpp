#include "View\SimViewStateEP.h"
#include "View\glAtrium.h"
#include <boost\lexical_cast.hpp>
SimViewStateEP* SimViewStateEP::_instance = nullptr;

SimViewStateEP::SimViewStateEP()
{
	_probeOnTheMove = -1;
	setPalette(DP_HOTTOCOLD);
}
SimViewStateEP::~SimViewStateEP()
{
}
//--------------------------------------------------
SimViewState* SimViewStateEP::Instance()
{
	if (SimViewStateEP::_instance == nullptr)
	{
		_instance = new SimViewStateEP();
	}

	return _instance;
}
void SimViewStateEP::setOutlineStyle(const  BRUSH_OUTLINE outline)
{

}
//--------------------------------------------------
int SimViewStateEP::findElectrode(glAtrium* view, Oscillator* src, Oscillator* osc)
{
	int found = -1;
	int temp;
	m_isSearchedMap[osc->oscillatorID] = true;


	found = view->linkToMachine->stimulator->isProperProbe(osc->oscillatorID);
	
	if (found == -1)
	{
		double distance;
		for (short k = 0; k < osc->m_neighbours.size(); ++k)
		{
			distance = sqrt(std::pow((src->m_x - osc->m_neighbours[k]->m_x), 2) +
				std::pow((src->m_y - osc->m_neighbours[k]->m_y), 2) +
				std::pow((src->m_z - osc->m_neighbours[k]->m_z), 2));

			if (distance <= cursorRadius * 4 && !(m_isSearchedMap[osc->m_neighbours[k]->oscillatorID]))
			{
				temp = findElectrode(view, src, osc->m_neighbours[k]);
				if (temp != -1)
					found = temp;
			}
		}
	}
	return found;
}
//--------------------------------------------------
void SimViewStateEP::paintCursor(glAtrium* view)
{
	view->setCursor(Qt::OpenHandCursor);
}
//--------------------------------------------------
void SimViewStateEP::handleMouseLeftPress(glAtrium* view, QMouseEvent *event)
{
	view->setCursor(Qt::ClosedHandCursor);
	view->setLastPos(event->pos());
	double 	y = event->pos().y();
	double 	x = event->pos().x();
	double height = static_cast<double>(view->height());
	double width = static_cast<double>(view->width());

	view->directionRay = view->screenToWorld(x, y, width, height);
	int item = view->itemAt(view->directionRay.x, view->directionRay.y, view->directionRay.z);

	if (item != -1)
	{
		m_isSearchedMap.clear();
		_probeOnTheMove = findElectrode(view, view->linkToMesh->m_mesh[item], view->linkToMesh->m_mesh[item]);
	}	
}
void SimViewStateEP::handleMouseRightPress(glAtrium* view, QMouseEvent *event)
{
	view->setLastPos(event->pos());
	_quatStart = _quat;
}
void SimViewStateEP::handleMouseRelease(glAtrium* view, QMouseEvent *event)
{
	view->setCursor(Qt::OpenHandCursor);
	view->setLastPos(event->pos());
	_probeOnTheMove = -1;
}
void SimViewStateEP::handleMousewheel(glAtrium* view, QWheelEvent *event)
{
	double numDegrees = static_cast<double>(event->delta()) / 350.0;

	view->distanceToCamera -= view->zoomingSpeed*numDegrees;
	paintCursor(view);

	event->accept();

}
void SimViewStateEP::handleMouseMove(glAtrium* view, QMouseEvent *event)
{
	double 	y = event->pos().y();
	double 	x = event->pos().x();
	double height = static_cast<double>(view->height());
	double width = static_cast<double>(view->width());

	view->directionRay = view->screenToWorld(x, y, width, height);

	if ((event->buttons() & Qt::LeftButton) && _probeOnTheMove !=-1)
	{
		int item = view->itemAt(view->directionRay.x, view->directionRay.y, view->directionRay.z);

		if (item != -1)
		{
			view->linkToMachine->stimulator->setProbeElectrode(view->linkToMesh, _probeOnTheMove, item);
			//XXXX
			if (_probeOnTheMove == 0) {
				view->linkToMachine->stimulator->setStimulationSiteID(item);
			}
		}
	}

	if ((event->buttons() & Qt::RightButton))
	{
		Vector3 next = view->screenToWorld(event->x(), event->y(), view->viewWidth, view->viewHeight);
		Vector3 previous = view->screenToWorld(view->lastPos.x(), view->lastPos.y(), view->viewWidth, view->viewHeight);

		next = get_arcball_vector(view->width(), view->height(), event->x(), event->y());
		previous = get_arcball_vector(view->width(),view->height(), view->lastPos.x(), view->lastPos.y());

		computeIncremental(view, previous, next);

		GLfloat m[16];

		//Matrix4 rotation;
		quaternionToMatrix(_quat, m);
		view->modelRotation = m;
	}
	view->updateGL();
}