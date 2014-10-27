#include "View\SimViewStateStructure.h"
#include "glAtrium.h"

SimViewStateStructure* SimViewStateStructure::_instance = nullptr;

SimViewStateStructure::SimViewStateStructure(glAtrium* view)
{
	cursorRadius = 2.0;
	_view = view;
	paintCursor(_view, cursorRadius);
	m_paintType = SOLID_WALL;
	m_clearType = ATRIAL_V3;
	m_currentDrawType = m_paintType;
}


SimViewStateStructure::~SimViewStateStructure()
{
}

SimViewState* SimViewStateStructure::Instance(glAtrium* view)
{
	if (SimViewStateStructure::_instance == nullptr)
	{
		_instance = new SimViewStateStructure(view);
	}
	return _instance;

}

void SimViewStateStructure::paintStructureInRadius(Oscillator* src, Oscillator* osc, const double radius, CELL_TYPE type)
{
	m_isPaintedMap[osc->oscillatorID] = true;
	
	double distance;
	for (short k = 0; k < osc->m_neighbours.size(); ++k)
	{
		distance = sqrt(std::pow((src->m_x - osc->m_neighbours[k]->m_x), 2) +
			std::pow((src->m_y - osc->m_neighbours[k]->m_y), 2) +
			std::pow((src->m_z - osc->m_neighbours[k]->m_z), 2));
		if (distance <= radius && !(m_isPaintedMap[osc->m_neighbours[k]->oscillatorID]))
		{
			paintStructureInRadius(src, osc->m_neighbours[k], radius, type);
		}
	}
	osc->m_type = type;
	if (type == SOLID_WALL)
		osc->m_v_electrogram = osc->vmax;
	else
		osc->m_v_electrogram = osc->m_v_scaledPotential;
}

QCursor  SimViewStateStructure::paintCursor(glAtrium* view, float radius)
{
	int crossSize = 5;
	int sizepix = floor(abs(radius*view->height() * view->frustrumSize * view->nearClippingPlaneDistance / (view->distanceToCamera)));
	QPixmap* m_LPixmap = new QPixmap(sizepix+2, sizepix+2);
	m_LPixmap->fill(Qt::transparent);
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
	m_currentDrawType = m_paintType;
	handleMouseMove(view, event);
}
void SimViewStateStructure::handleMouseRightPress(glAtrium* view, QMouseEvent *event)
{
	m_currentDrawType = m_clearType;
	handleMouseMove(view, event);
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


	cursorRadius *= (1+0.1*numDegrees);
	view->setCursor(paintCursor(view, cursorRadius));

	event->accept();
	view->updateGL();
	view->resizeGL(view->width(), view->height());

}
void SimViewStateStructure::handleMouseMove(glAtrium* view, QMouseEvent *event)
{
	if ((event->buttons()))
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

			paintStructureInRadius(view->linkToMesh->m_mesh[item], view->linkToMesh->m_mesh[item], cursorRadius, m_currentDrawType);
		}

		view->lastPos = event->pos();
		m_isPaintedMap.clear();
	}
	view->updateGL();
}