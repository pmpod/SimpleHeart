#include "View\SimViewStateStructure.h"
#include "glAtrium.h"

SimViewStateStructure* SimViewStateStructure::_instance = nullptr;

SimViewStateStructure::SimViewStateStructure()
{
}


SimViewStateStructure::~SimViewStateStructure()
{
}

SimViewState* SimViewStateStructure::Instance()
{
	if (SimViewStateStructure::_instance == nullptr)
	{
		_instance = new SimViewStateStructure;
	}

	return _instance;

}

void SimViewStateStructure::paintStructureInRadius(Oscillator* src, Oscillator* osc, const double radius, CELL_TYPE type)
{
	if (!osc->m_underStimulation)
	{
		m_painted.push_back(osc);
		osc->m_underStimulation = true;
	}
	double distance;
	for (short k = 0; k < osc->m_neighbours.size(); ++k)
	{
		distance = std::pow((src->m_x - osc->m_neighbours[k]->m_x), 2) +
			std::pow((src->m_y - osc->m_neighbours[k]->m_y), 2) +
			std::pow((src->m_z - osc->m_neighbours[k]->m_z), 2);
		if (distance <= radius && !(osc->m_neighbours[k]->m_underStimulation))
		{
			paintStructureInRadius(src, osc->m_neighbours[k], radius, type);
		}
	}
	osc->m_type = type;
	osc->m_v_electrogram = osc->vmax;
}

void SimViewStateStructure::handleMouseLeftPress(glAtrium* view, QMouseEvent *event)
{
	Vector3 vec= view->screenToWorld(5, 5, view->width(), view->height());

	QPixmap* m_LPixmap = new QPixmap(32, 32);
	m_LPixmap->fill(Qt::transparent); // Otherwise you get a black background :(
	QPainter painter(m_LPixmap);
	QColor red(255, 0, 0, 128);

	painter.setPen(Qt::NoPen);        // Otherwise you get an thin black border
	painter.setBrush(red);

	painter.drawEllipse(0, 0, 32, 32);
	QCursor m_Cursor = QCursor(*m_LPixmap);
	view->setCursor(m_Cursor);

	view->setLastPos(event->pos());

	double 	y = event->pos().y();
	double 	x = event->pos().x();
	double height = static_cast<double>(view->height());
	double width = static_cast<double>(view->width());

	view->directionRay = view->screenToWorld(x, y, width, height);

	int item = view->itemAt(view->directionRay.x, view->directionRay.y, view->directionRay.z);
	if (item != -1)
	{
		view->testProbe.x = view->linkToMesh->m_mesh[item]->m_x;
		view->testProbe.y = view->linkToMesh->m_mesh[item]->m_y;
		view->testProbe.z = view->linkToMesh->m_mesh[item]->m_z;

		paintStructureInRadius(view->linkToMesh->m_mesh[item], view->linkToMesh->m_mesh[item], 6, SOLID_WALL);
	}
	m_painted.clear();






}
void SimViewStateStructure::handleMouseRightPress(glAtrium* view, QMouseEvent *event)
{

	view->setLastPos(event->pos());

	double 	y = event->pos().y();
	double 	x = event->pos().x();
	double height = static_cast<double>(view->height());
	double width = static_cast<double>(view->width());

	view->directionRay = view->screenToWorld(x, y, width, height);

	int item = view->itemAt(view->directionRay.x, view->directionRay.y, view->directionRay.z);
	if (item != -1)
	{
		view->testProbe.x = view->linkToMesh->m_mesh[item]->m_x;
		view->testProbe.y = view->linkToMesh->m_mesh[item]->m_y;
		view->testProbe.z = view->linkToMesh->m_mesh[item]->m_z;
		view->linkToMachine->m_stimulationID = view->itemAt(view->directionRay.x, view->directionRay.y, view->directionRay.z);
		view->linkToMachine->m_definitions->m_ectopicActivity = true;
	}
	view->setLastPos(event->pos());
}
void SimViewStateStructure::handleMouseRelease(glAtrium* view, QMouseEvent *event)
{
	view->linkToMachine->m_definitions->m_ectopicActivity = false;

	view->setLastPos(event->pos());
	view->linkToMesh->clearWallCells();
	view->linkToMesh->setWallCells();
}
void SimViewStateStructure::handleMousewheel(glAtrium* view, QWheelEvent *event)
{
	double numDegrees = static_cast<double>(event->delta()) / 350.0;


	view->distanceToCamera -= view->zoomingSpeed*numDegrees;
	//if (distanceToCamera <nearClippingPlaneDistance) distanceToCamera = nearClippingPlaneDistance;

	////if (event->orientation() == Qt::Horizontal) {
	//frustrumSize -= numDegrees;
	//if (frustrumSize <0.1) frustrumSize = 0.1;

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	//nearClippingPlaneDistance = 2.0;
	//glFrustum(-frustrumSize*static_cast<double>(this->width()) / static_cast<double>(this->height()),
	//	+frustrumSize*(static_cast<double>(this->width()) / static_cast<double>(this->height())),
	//	+frustrumSize, -frustrumSize, nearClippingPlaneDistance, 50.0);
	//fov = 2 * atan((frustrumSize - (-frustrumSize))*0.5 / nearClippingPlaneDistance);
	//glMatrixMode(GL_MODELVIEW);
	event->accept();
	////	}

}
void SimViewStateStructure::handleMouseMove(glAtrium* view, QMouseEvent *event)
{
	if ((event->buttons() & Qt::LeftButton))
	{


		double 	y = event->pos().y();
		double 	x = event->pos().x();
		double height = static_cast<double>(view->height());
		double width = static_cast<double>(view->width());

		view->directionRay = view->screenToWorld(x, y, width, height);

		int item = view->itemAt(view->directionRay.x, view->directionRay.y, view->directionRay.z);
		if (item != -1)
		{
			view->testProbe.x = view->linkToMesh->m_mesh[item]->m_x;
			view->testProbe.y = view->linkToMesh->m_mesh[item]->m_y;
			view->testProbe.z = view->linkToMesh->m_mesh[item]->m_z;

			paintStructureInRadius(view->linkToMesh->m_mesh[item], view->linkToMesh->m_mesh[item], 6, SOLID_WALL);
		}

		view->lastPos = event->pos();
		m_painted.clear();
	}
	view->updateGL();
}