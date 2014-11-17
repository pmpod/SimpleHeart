#include "View\SimViewStateEP.h"
#include "View\glAtrium.h"
#include <boost\lexical_cast.hpp>
SimViewStateEP* SimViewStateEP::_instance = nullptr;

SimViewStateEP::SimViewStateEP()
{
	cursorRadius = 1.0;
	_dataDisplayMode = 1;//POTENTIAL
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
//--------------------------------------------------
void SimViewStateEP::setDisplayMode(const int mode)
{
	_dataDisplayMode = mode;
}\
void SimViewStateEP::setOutlineStyle(const  BRUSH_OUTLINE outline)
{

}
void SimViewStateEP::paintLegend(glAtrium* view)
{
	glCallList(view->displayListIndex);
	GLfloat widthL = view->frustrumSize*_scale*0.5;
	GLfloat heightL = view->frustrumSize*_scale*0.2;
	view->renderText(widthL*1.4f, 16.0*heightL - heightL / 2, 0, "-75 mV", QFont(), view->displayListIndex);
	view->renderText(widthL*1.4f, -16.0*heightL - heightL / 2, 0, "25 mV", QFont(), view->displayListIndex);
	view->renderText(widthL*1.4f, 0.0*heightL - heightL / 2, 0, "0", QFont(), view->displayListIndex);
}
void SimViewStateEP::prepareLegend(glAtrium* view)
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
		glColor3f(col[0], col[1], col[2]);
		glVertex3f(widthL, i*heightL, 0.0f);
		glVertex3f(-widthL, i*heightL, 0.0f);

	}
	glEnd();
	glBegin(GL_LINES);
	glColor3fv(col_g);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, col_g);
	for (GLfloat i = -15.0; i <= 15.0; ++i)
	{
		glVertex3f(-widthL, i*heightL, 0.0f);
		glVertex3f(widthL, i*heightL, 0.0f);

	}
	glEnd();

	glBegin(GL_LINES);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, col_w);
	glColor3fv(col_w);
	glVertex3f(-widthL, -16.0*heightL, 0.0f);
	glVertex3f(widthL*1.2f, -16.0*heightL, 0.0f);
	glVertex3f(-widthL, 0.0*heightL, 0.0f);
	glVertex3f(widthL*1.2f, 0.0*heightL, 0.0f);
	glVertex3f(-widthL, 16.0*heightL, 0.0f);
	glVertex3f(widthL*1.2f, 16.0*heightL, 0.0f);
	glEnd();

	glEndList();

}
//--------------------------------------------------
int SimViewStateEP::findElectrode(glAtrium* view, Oscillator* src, Oscillator* osc)
{
	int found = -1;
	int temp;
	m_isSearchedMap[osc->oscillatorID] = true;


	for (short k = 0; k < view->linkToMachine->probeOscillator.size(); ++k)
	{
		ProbeElectrode *probe = view->linkToMachine->probeOscillator[k];
		if (probe->getOscillatorID() == osc->oscillatorID)
		{
			found = k;
		}
	}
	if (found == -1)
	{
		double distance;
		for (short k = 0; k < osc->m_neighbours.size(); ++k)
		{
			distance = sqrt(std::pow((src->m_x - osc->m_neighbours[k]->m_x), 2) +
				std::pow((src->m_y - osc->m_neighbours[k]->m_y), 2) +
				std::pow((src->m_z - osc->m_neighbours[k]->m_z), 2));

			if (distance <= cursorRadius * 2 && !(m_isSearchedMap[osc->m_neighbours[k]->oscillatorID]))
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
void SimViewStateEP::paintModel(glAtrium* view)
{
	CardiacMesh *msh = view->linkToMesh;
	std::vector<Oscillator*> oscs = view->linkToMesh->m_mesh;
	int vertexNumber = msh->m_vertexList.size();

	GLfloat val1;
	GLfloat vmin = msh->m_minElectrogram;
	GLfloat vmax = msh->m_maxElectrogram;
	
	for (int currentVertex = 0; currentVertex < oscs.size(); ++currentVertex)
	{
		if (oscs[currentVertex]->m_type != SOLID_WALL)
		{
			switch (_dataDisplayMode)
			{
			case 1:
				val1 = oscs[currentVertex]->m_v_scaledPotential;
				break;
			case 2:
				val1 = oscs[currentVertex]->getLastCurrentSource();
				break;
			case 3:
				val1 = oscs[currentVertex]->m_v_current[0];
				break;
			case 4:
				val1 = oscs[currentVertex]->m_currentTime - oscs[currentVertex]->m_previousTime;
				break;
			case 5:
				val1 = oscs[currentVertex]->m_v_electrogram;
				break;
			}
			colorMap(val1, vmin, vmax, msh->m_vertexMatrix[currentVertex].r, msh->m_vertexMatrix[currentVertex].g, msh->m_vertexMatrix[currentVertex].b);

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
		
		glDrawElements(GL_TRIANGLES, view->linkToMesh->m_vertexList.size()*3, GL_UNSIGNED_INT, view->linkToMesh->m_indicesMatrix);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glPushMatrix();


	// [3] Paint probes
		for (short n = 0; n < view->linkToMachine->probeOscillator.size(); ++n)
		{
			drawSphere(0.4, 10, 10, view->linkToMachine->probeOscillator[n]->getPositionX(),
				view->linkToMachine->probeOscillator[n]->getPositionY(),
				view->linkToMachine->probeOscillator[n]->getPositionZ(), 1.0f / (n + 1), 1.0f / (n + 1), 1.0f);

		
			view->renderText(view->linkToMachine->probeOscillator[n]->getPositionX(),
				view->linkToMachine->probeOscillator[n]->getPositionY(),
				view->linkToMachine->probeOscillator[n]->getPositionZ() + 1, "Electrode");
		}
		if (view->paintRay)
		{
			drawSphere(0.4, 10, 10, view->testProbe.x, view->testProbe.y, view->testProbe.z, 1.0f, 1.0f, 1.0f);
		}

	glPopMatrix();
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
			view->linkToMachine->probeOscillator[_probeOnTheMove]->setOscillator(view->linkToMesh->m_mesh[item]);
		}
	}

	if ((event->buttons() & Qt::RightButton))
	{
		Vector3 next = view->screenToWorld(event->x(), event->y(), view->viewWidth, view->viewHeight);
		Vector3 previous = view->screenToWorld(view->lastPos.x(), view->lastPos.y(), view->viewWidth, view->viewHeight);

		next = get_arcball_vector(view->width(), view->height(), event->x(), event->y());
		previous = get_arcball_vector(view->width(),view->height(), view->lastPos.x(), view->lastPos.y());

		computeIncremental(previous, next);

		GLfloat m[16];

		//Matrix4 rotation;
		quaternionToMatrix(_quat, m);
		view->modelRotation = m;
	}
	view->updateGL();
}