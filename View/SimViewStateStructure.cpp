#include "View\SimViewStateStructure.h"
#include "glAtrium.h"

SimViewStateStructure* SimViewStateStructure::_instance = nullptr;

SimViewStateStructure::SimViewStateStructure()
{
	cursorRadius = 2.0;
	m_paintType = SOLID_WALL;
	m_clearType = ATRIAL_V3;
	m_currentDrawType = m_paintType;


	//Colormap for structures
	m_structureColorMap[SOLID_WALL] = 0.0;
	m_structureColorMap[ATRIAL_V3] = 0.8;
}


SimViewStateStructure::~SimViewStateStructure()
{
}
void SimViewStateStructure::setDisplayMode(const int mode)
{

}
SimViewState* SimViewStateStructure::Instance()
{
	if (SimViewStateStructure::_instance == nullptr)
	{
		_instance = new SimViewStateStructure();
	}
	//_instance->paintCursor(view, _instance->cursorRadius);
	return _instance;

}
void SimViewStateStructure::paintModel(glAtrium* view)
{
	CardiacMesh *msh = view->linkToMesh;
	glPushMatrix();

	int vertexNumber = msh->m_vertexList.size();
	int t_ID1;
	int t_ID2;
	int t_ID3;

	GLfloat val;
	GLfloat color[] = { 0.f, 0.f, 0.f, 1.f };
	GLfloat xx, yy, zz;

	GLfloat vmin = msh->m_minElectrogram;
	GLfloat vmax = msh->m_maxElectrogram;

	for (unsigned int j = 0; j < vertexNumber; j = j + 1)
	{
		//if (linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_type != SOLID_WALL)
		//{
		t_ID1 = msh->m_vertexList[j]->id_1;
		t_ID2 = msh->m_vertexList[j]->id_2;
		t_ID3 = msh->m_vertexList[j]->id_3;

		glBegin(GL_TRIANGLES);

		m_structureColorMap[SOLID_WALL] = 0.0;
		m_structureColorMap[ATRIAL_V3] = 0.8;

		paintCellTriangle(msh->m_mesh[t_ID1]->m_x,
			msh->m_mesh[t_ID1]->m_y,
			msh->m_mesh[t_ID1]->m_z, m_structureColorMap[msh->m_mesh[t_ID1]->m_type],
			msh->m_mesh[t_ID2]->m_x,
			msh->m_mesh[t_ID2]->m_y,
			msh->m_mesh[t_ID2]->m_z, m_structureColorMap[msh->m_mesh[t_ID2]->m_type],
			msh->m_mesh[t_ID3]->m_x,
			msh->m_mesh[t_ID3]->m_y,
			msh->m_mesh[t_ID3]->m_z, m_structureColorMap[msh->m_mesh[t_ID3]->m_type],
			3, 0, 1);
		glEnd();

	}
	if (view->paintRay)
	{
		drawSphere(0.2, 10, 10, view->testProbe.x, view->testProbe.y, view->testProbe.z, 1.0f, 1.0f, 1.0f);
	}
	glPopMatrix();






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
	//if (type == SOLID_WALL)
	//	osc->m_v_electrogram = osc->vmax;
	//else
	//	osc->m_v_electrogram = osc->m_v_scaledPotential;
}

void  SimViewStateStructure::paintCursor(glAtrium* view)
{
	int crossSize = 5;
	int sizepix = floor(abs(cursorRadius*view->height() * view->frustrumSize * view->nearClippingPlaneDistance / (view->distanceToCamera)));
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

	view->setCursor(QCursor(*m_LPixmap));
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
	paintCursor(view);

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