#include "View\SimViewStateDiffusion.h"
#include "View\glAtrium.h"

double gaussFunction(double y, double mean, double sigma)
{
	double norm = 1 / sqrt(2 * 3.14) * sigma;
	double expon = exp((-pow((y - mean), 2)) / (2 * pow(sigma, 2)));

	return norm*expon;
}

SimViewStateDiffusion* SimViewStateDiffusion::_instance = nullptr;

SimViewStateDiffusion::SimViewStateDiffusion()
{
	cursorRadius = 3.0;
	setPalette(DP_COLD);

	_gaussSigma = 3.0;
	_maxDiffusion =0.05;
	_minDiffusion =0.0;
	_paintValue = 0.01;
	_outline = BRUSH_GAUSS;
}
SimViewStateDiffusion::~SimViewStateDiffusion()
{
}
SimViewState* SimViewStateDiffusion::Instance()
{
	if (SimViewStateDiffusion::_instance == nullptr)
	{
		_instance = new SimViewStateDiffusion();
	}
	return _instance;

}
//--------------------------------------------------
void SimViewStateDiffusion::setDisplayMode(const int mode)
{

}
//--------------------------------------------------
void SimViewStateDiffusion::paintCursor(glAtrium* view)
{
	int crossSize = 5;
	int sizepix = floor(abs(cursorRadius*view->height() * view->frustrumSize * view->nearClippingPlaneDistance / (view->distanceToCamera)));

	int pixmapsize;
	sizepix < 60 ? pixmapsize = 62 : pixmapsize = sizepix + 2;
	QPixmap* m_LPixmap = new QPixmap(pixmapsize, pixmapsize);
	m_LPixmap->fill(Qt::transparent);
	QPainter painter(m_LPixmap);
	QColor col(128, 64, 64, 128);

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
	sprintf(buff, "%1.1f", cursorRadius * 2);
	std::string str(buff);
	painter.drawText(floor(3 + sizepix / 2.0), floor(-2 + sizepix / 2.0), QString(str.c_str()) + "mm");
	view->setCursor(QCursor(*m_LPixmap));
}
void SimViewStateDiffusion::paintLegend(glAtrium* view)
{
	glCallList(view->displayListIndex);
	GLfloat widthL = view->frustrumSize*_scale*0.5;
	GLfloat heightL = view->frustrumSize*_scale*0.2;
	view->renderText(widthL*1.4f, 16.0*heightL - heightL / 2, 0, "CV = 0", QFont(), view->displayListIndex);
	view->renderText(widthL*1.4f, -16.0*heightL - heightL / 2, 0, "CV = MAX", QFont(), view->displayListIndex);

}
void SimViewStateDiffusion::prepareLegend(glAtrium* view)
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
void SimViewStateDiffusion::paintModel(glAtrium* view)
{

	CardiacMesh *msh = view->linkToMesh;
	std::vector<Oscillator*> oscs = view->linkToMesh->m_mesh;
	int vertexNumber = msh->m_vertexList.size();

	GLfloat val;

	for (int currentVertex = 0; currentVertex < oscs.size(); ++currentVertex)
	{
		if (oscs[currentVertex]->m_type != SOLID_WALL)
		{
			val = oscs[currentVertex]->m_sigmaX;

			colorMap(val, _minDiffusion, _maxDiffusion, msh->m_vertexMatrix[currentVertex].r, msh->m_vertexMatrix[currentVertex].g, msh->m_vertexMatrix[currentVertex].b);
		}
		else
		{
			msh->m_vertexMatrix[currentVertex].r = 0.1;
			msh->m_vertexMatrix[currentVertex].g = 0.1;
			msh->m_vertexMatrix[currentVertex].b = 0.1;
		}
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

void SimViewStateDiffusion::setOutlineStyle(const  BRUSH_OUTLINE outline)
{
	_outline = outline;
}
//--------------------------------------------------
void SimViewStateDiffusion::paintDiffusionInRadius(Oscillator* src, Oscillator* osc)
{
	m_isPaintedMap[osc->oscillatorID] = true;
	
	double distance;
	for (short k = 0; k < osc->m_neighbours.size(); ++k)
	{
		distance = sqrt(std::pow((src->m_x - osc->m_neighbours[k]->m_x), 2) +
			std::pow((src->m_y - osc->m_neighbours[k]->m_y), 2) +
			std::pow((src->m_z - osc->m_neighbours[k]->m_z), 2));

		if (distance <= cursorRadius*2 && !(m_isPaintedMap[osc->m_neighbours[k]->oscillatorID]))
		{
			paintDiffusionInRadius(src, osc->m_neighbours[k]);
		}
	}

	double val = osc->m_sigmaX;
	double valsign = (_paintValue >= osc->m_sigmaX) ? +1.0 : -1.0;
	distance = sqrt(std::pow((src->m_x - osc->m_x), 2) +
		std::pow((src->m_y - osc->m_y), 2) +
		std::pow((src->m_z - osc->m_z), 2));
	switch (_outline)
	{
	case BRUSH_GAUSS:
		val += valsign* gaussFunction(distance, 0, cursorRadius/2) / (cursorRadius*100.0);
		if (valsign * val >= valsign *_paintValue) val = _paintValue;
		break;
	case BRUSH_UNI:
		if (distance <= cursorRadius) 
			val = _paintValue;

	}

	osc->setSigma(val, val, val);
}
//--------------------------------------------------
void SimViewStateDiffusion::handleMouseLeftPress(glAtrium* view, QMouseEvent *event)
{
	_paintValue = 0.01;
	handleMouseMove(view, event);
}
void SimViewStateDiffusion::handleMouseRightPress(glAtrium* view, QMouseEvent *event)
{
	_paintValue = 0.05;
	handleMouseMove(view, event);
}
void SimViewStateDiffusion::handleMouseRelease(glAtrium* view, QMouseEvent *event)
{
	view->setLastPos(event->pos());
	//view->linkToMesh->clearWallCells();
	//view->linkToMesh->setWallCells();

}
void SimViewStateDiffusion::handleMousewheel(glAtrium* view, QWheelEvent *event)
{
	double numDegrees = static_cast<double>(event->delta()) / 350.0;


	cursorRadius *= (1+0.1*numDegrees);
	paintCursor(view);

	event->accept();
	view->updateGL();
	view->resizeGL(view->width(), view->height());

}
void SimViewStateDiffusion::handleMouseMove(glAtrium* view, QMouseEvent *event)
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



			//view->linkToMesh->structureUpdated = true;
			paintDiffusionInRadius(view->linkToMesh->m_mesh[item], view->linkToMesh->m_mesh[item]);
		}

		view->lastPos = event->pos();
		m_isPaintedMap.clear();
	}
	view->updateGL();
}