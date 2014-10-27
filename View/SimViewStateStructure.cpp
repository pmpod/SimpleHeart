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

QCursor  SimViewStateStructure::paintCursor(glAtrium* view, float radius)
{
	double vLength = abs(radius*view->height()*view->frustrumSize / (view->distanceToCamera));
	double hLength = vLength*view->width() / view->height();

	int crossSize = 5;
	int sizepix = floor(vLength);
	QPixmap* m_LPixmap = new QPixmap(sizepix+2, sizepix+2);
	m_LPixmap->fill(Qt::transparent); // Otherwise you get a black background :(
	QPainter painter(m_LPixmap);
	QColor col(64, 64, 64, 128);

	QPen pen;  // creates a default pen
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(1);
	pen.setBrush(QColor(128, 128, 128, 128));
	painter.setPen(pen);
	painter.setBrush(col);

	painter.drawLine(floor(1 + sizepix / 2.0) - crossSize, floor(1 + sizepix / 2.0), floor(1 + sizepix / 2.0) + crossSize, floor(1 + sizepix / 2.0));
	painter.drawLine(floor(1 + sizepix / 2.0), floor(1 + sizepix / 2.0) - crossSize, floor(1 + sizepix / 2.0), floor(1 + sizepix / 2.0) + crossSize);
	painter.drawEllipse(1, 1, sizepix, sizepix);
	return QCursor(*m_LPixmap);
}
void SimViewStateStructure::handleMouseLeftPress(glAtrium* view, QMouseEvent *event)
{
	double radius = 4;
	view->setCursor(paintCursor(view, radius));

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

		paintStructureInRadius(view->linkToMesh->m_mesh[item], view->linkToMesh->m_mesh[item], radius, SOLID_WALL);
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
	view->updateGL();
	view->resizeGL(view->width(), view->height());
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