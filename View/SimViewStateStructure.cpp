#include "View\SimViewStateStructure.h"
#include "View\glAtrium.h"

SimViewStateStructure* SimViewStateStructure::_instance = nullptr;

SimViewStateStructure::SimViewStateStructure()
{
	cursorRadius = 2.0;
	m_paintType = SOLID_WALL;
	m_clearType = ATRIAL_V3;
	m_currentDrawType = m_paintType;
	setPalette(DP_GRAY);


	//Colormap for structures
	m_structureColorMap[SOLID_WALL] = 0.0;
	m_structureColorMap[ATRIAL_V3] = 24;
}
SimViewStateStructure::~SimViewStateStructure()
{
}
SimViewState* SimViewStateStructure::Instance()
{
	if (SimViewStateStructure::_instance == nullptr)
	{
		_instance = new SimViewStateStructure();
	}
	return _instance;

}
//--------------------------------------------------
void SimViewStateStructure::setMode(const int mode)
{

}
void SimViewStateStructure::setOutlineStyle(const  BRUSH_OUTLINE outline)
{

}
//--------------------------------------------------
void SimViewStateStructure::paintCursor(glAtrium* view)
{
	int crossSize = 5;
	int sizepix = floor(abs(cursorRadius*view->height() * view->frustrumSize * view->nearClippingPlaneDistance / (view->distanceToCamera)));

	int pixmapsize;
	sizepix < 60 ? pixmapsize = 62 : pixmapsize = sizepix + 2;
	QPixmap* m_LPixmap = new QPixmap(pixmapsize, pixmapsize);
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

	pen.setBrush(QColor(255, 255, 255, 255));
	painter.setPen(pen);
	char buff[20];
	sprintf(buff, "%1.1f", cursorRadius*2);
	std::string str(buff);
	painter.drawText(floor(3 + sizepix / 2.0), floor(-2 + sizepix / 2.0), QString(str.c_str()) + "mm");
	view->setCursor(QCursor(*m_LPixmap));
}
void SimViewStateStructure::paintLegend(glAtrium* view)
{
	glCallList(view->displayListIndex);
	GLfloat widthL = view->frustrumSize*_scale*0.5;
	GLfloat heightL = view->frustrumSize*_scale*0.2;
	view->renderText(widthL*1.4f, 16.0*heightL - heightL / 2, 0, "WALL", QFont(), view->displayListIndex);
	view->renderText(widthL*1.4f, -16.0*heightL - heightL / 2, 0, "ATRIUM", QFont(), view->displayListIndex);

}
void SimViewStateStructure::prepareLegend(glAtrium* view)
{

	float f[1];
	GLfloat widthL = view->frustrumSize*_scale*0.5;
	GLfloat heightL = view->frustrumSize*_scale*0.2;

	GLfloat col_w[] = { 1.0, 1.0, 1.0, 1.f };
	GLfloat col_g[] = { .5f, .5f, .5f, .5f };
	GLfloat col[] = { 0.0, 0.0, 1.0, 1.f };

	GLfloat ccol;
	// compile the display list, store a triangle in it
	glNewList(view->displayListIndex, GL_COMPILE);

	glBegin(GL_QUAD_STRIP);
	for (GLfloat i = -16.0; i <= 16.0; ++i)
	{
		ccol = -i;

		colorMap(ccol, -16.0, 16.0, col[0], col[1], col[2]);
		glColor3fv(col);
		glVertex3f(widthL, i*heightL, 0.0f);
		glVertex3f(-widthL, i*heightL, 0.0f);

	}
	glEnd();
	glBegin(GL_LINES);
	glColor3fv(col_g);
	for (GLfloat i = -15.0; i <= 15.0; ++i)
	{
		glVertex3f(-widthL, i*heightL, 0.0f);
		glVertex3f(widthL, i*heightL, 0.0f);

	}
	glEnd();

	glBegin(GL_LINES);
	glColor3fv(col_w);
	glVertex3f(-widthL, -16.0*heightL, 0.0f);
	glVertex3f(widthL*1.2f, -16.0*heightL, 0.0f);
	glVertex3f(-widthL, 0.0*heightL, 0.0f);
	glVertex3f(widthL*1.2f, 0.0*heightL, 0.0f);
	glVertex3f(-widthL, 16.0*heightL, 0.0f);
	glVertex3f(widthL*1.2f, 16.0*heightL, 0.0f);
	glEnd();

	glColor3fv(col_w);
	glEndList();

}
void SimViewStateStructure::paintModel(glAtrium* view)
{

	CardiacMesh *msh = view->linkToMesh;
	std::vector<Oscillator*> oscs = view->linkToMesh->m_mesh;
	int vertexNumber = msh->m_vertexList.size();

	GLfloat val;
	GLfloat vmin = 0;
	GLfloat vmax = 1;

	for (int currentVertex = 0; currentVertex < oscs.size(); ++currentVertex)
	{
		val = m_structureColorMap[oscs[currentVertex]->m_type];
		colorMap(val, vmin, vmax, msh->m_vertexMatrix[currentVertex].r, msh->m_vertexMatrix[currentVertex].g, msh->m_vertexMatrix[currentVertex].b);

	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

		glColorPointer(3, GL_FLOAT, sizeof(SVertex), &view->linkToMesh->m_vertexMatrix[0].r);

		glVertexPointer(3, GL_FLOAT, sizeof(SVertex), view->linkToMesh->m_vertexMatrix);

		glDrawElements(GL_TRIANGLES, view->linkToMesh->m_vertexList.size() * 3, GL_UNSIGNED_INT, view->linkToMesh->m_indicesMatrix);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);


	if (view->paintRay)
	{
		drawSphere(0.2, 10, 10, view->testProbe.x, view->testProbe.y, view->testProbe.z, 1.0f, 1.0f, 1.0f);
	}






}
//--------------------------------------------------
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
}
//--------------------------------------------------
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
			view->linkToMesh->structureUpdated = true;
			paintStructureInRadius(view->linkToMesh->m_mesh[item], view->linkToMesh->m_mesh[item], cursorRadius, m_currentDrawType);
		}

		view->lastPos = event->pos();
		m_isPaintedMap.clear();
	}
	view->updateGL();
}