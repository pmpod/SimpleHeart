#include "View\SimViewStateEP.h"
#include "View\glAtrium.h"
#include <boost\lexical_cast.hpp>
SimViewStateEP* SimViewStateEP::_instance = nullptr;

SimViewStateEP::SimViewStateEP()
{
	cursorRadius = 2.0;
	_dataDisplayMode = 1;//POTENTIAL
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
void SimViewStateEP::paintCursor(glAtrium* view)
{
	int crossSize = 5;
	int sizepix = floor(abs(cursorRadius*view->height() * view->frustrumSize * view->nearClippingPlaneDistance / (view->distanceToCamera)));

	int pixmapsize;
	sizepix < 60 ? pixmapsize = 62 : pixmapsize = sizepix + 2;
	QPixmap* m_LPixmap = new QPixmap(pixmapsize, pixmapsize);
	m_LPixmap->fill(Qt::transparent);
	QPainter painter(m_LPixmap);
	QColor col(0, 0, 64, 128);

	QPen pen;  // creates a default pen
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(2);
	pen.setBrush(QColor(128, 128, 128, 128));
	painter.setPen(pen);
	painter.setBrush(col);

	painter.drawEllipse(1, 1, sizepix, sizepix);
	painter.drawLine(floor(1 + sizepix / 2.0) - crossSize, floor(1 + sizepix / 2.0), floor(1 + sizepix / 2.0) + crossSize, floor(1 + sizepix / 2.0));
	painter.drawLine(floor(1 + sizepix / 2.0), floor(1 + sizepix / 2.0) - crossSize, floor(1 + sizepix / 2.0), floor(1 + sizepix / 2.0) + crossSize);

	painter.setBrush(QColor(256, 256, 256, 256));
	char buff[20];
	sprintf(buff, "%1.1f", cursorRadius * 2);
	std::string str(buff);
	painter.drawText(floor(3 + sizepix / 2.0), floor(-2 + sizepix / 2.0), QString(str.c_str()) + "mm");

	view->setCursor(QCursor(*m_LPixmap));
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

	//glPushMatrix();

	//// [1] Paint data

	//CardiacMesh *msh = view->linkToMesh;
	//std::vector<Oscillator*> oscs = view->linkToMesh->m_mesh;

	//int vertexNumber = msh->m_vertexList.size();

	//int t_ID1, t_ID2, t_ID3;
	//GLfloat val1, val2, val3;
	//GLfloat color[] = { 0.f, 0.f, 0.f, 1.f };
	//GLfloat vmin = msh->m_minElectrogram;
	//GLfloat vmax = msh->m_maxElectrogram;

	//glBegin(GL_TRIANGLES);
	//for (unsigned int j = 0; j < vertexNumber; j = j + 1)
	//{
	//	t_ID1 = msh->m_vertexList[j]->id_1;
	//	t_ID2 = msh->m_vertexList[j]->id_2;
	//	t_ID3 = msh->m_vertexList[j]->id_3;

	//	if (oscs[t_ID1]->m_type != SOLID_WALL)
	//	{

	//		switch (_dataDisplayMode)
	//		{
	//		case 1:
	//			val1 = oscs[t_ID1]->m_v_scaledPotential;
	//			val2 = oscs[t_ID2]->m_v_scaledPotential;
	//			val3 = oscs[t_ID3]->m_v_scaledPotential;
	//			break;
	//		case 2:
	//			val1 = oscs[t_ID1]->getLastCurrentSource();
	//			val2 = oscs[t_ID2]->getLastCurrentSource();
	//			val3 = oscs[t_ID3]->getLastCurrentSource();
	//			break;
	//		case 3:
	//			val1 = oscs[t_ID1]->m_v_current[0];
	//			val2 = oscs[t_ID2]->m_v_current[0];
	//			val3 = oscs[t_ID3]->m_v_current[0];
	//			break;
	//		case 4:
	//			val1 = oscs[t_ID1]->m_currentTime - oscs[t_ID1]->m_previousTime;
	//			val2 = oscs[t_ID2]->m_currentTime - oscs[t_ID2]->m_previousTime;
	//			val3 = oscs[t_ID3]->m_currentTime - oscs[t_ID3]->m_previousTime;
	//			break;
	//		case 5:
	//			val1 = oscs[t_ID1]->m_v_electrogram;
	//			val2 = oscs[t_ID2]->m_v_electrogram;
	//			val3 = oscs[t_ID3]->m_v_electrogram;
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		val1 = 0.0;
	//		val2 = 0.0;
	//		val3 = 0.0;
	//	}

	//	glBegin(GL_TRIANGLES);
	//	colorMap(val1, vmin, vmax, color[0], color[1], color[2]);
	//	glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
	//	glMaterialfv(GL_FRONT, GL_AMBIENT, color);
	//	//glNormal3d(x1, y1, z1);
	//	glVertex3f(msh->m_mesh[t_ID1]->m_x, msh->m_mesh[t_ID1]->m_y, msh->m_mesh[t_ID1]->m_z);

	//	colorMap(val2, vmin, vmax, color[0], color[1], color[2]);
	//	glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
	//	glMaterialfv(GL_FRONT, GL_AMBIENT, color);
	//	//glNormal3d(x2, y2, z2);
	//	glVertex3f(msh->m_mesh[t_ID2]->m_x, msh->m_mesh[t_ID2]->m_y, msh->m_mesh[t_ID2]->m_z);

	//	colorMap(val3, vmin, vmax, color[0], color[1], color[2]);
	//	glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
	//	glMaterialfv(GL_FRONT, GL_AMBIENT, color);
	//	//glNormal3d(x3, y3, z3);
	//	glVertex3f(msh->m_mesh[t_ID3]->m_x, msh->m_mesh[t_ID3]->m_y, msh->m_mesh[t_ID3]->m_z);
	//	glEnd();

	//}
	//glEnd();





	glPushMatrix();


	// [3] Paint probes
		for (short n = 0; n < view->linkToMachine->probeOscillator.size(); ++n)
		{
			drawSphere(0.2, 10, 10, view->linkToMachine->probeOscillator[n]->getPositionX(),
				view->linkToMachine->probeOscillator[n]->getPositionY(),
				view->linkToMachine->probeOscillator[n]->getPositionZ(), 1.0f / (n + 1), 1.0f / (n + 1), 1.0f);

		
			view->renderText(view->linkToMachine->probeOscillator[n]->getPositionX(),
				view->linkToMachine->probeOscillator[n]->getPositionY(),
				view->linkToMachine->probeOscillator[n]->getPositionZ() + 1, "Electrode");
		}
		if (view->paintRay)
		{
			drawSphere(0.2, 10, 10, view->testProbe.x, view->testProbe.y, view->testProbe.z, 1.0f, 1.0f, 1.0f);
		}

		glLoadIdentity();
		drawSphere(0.2, 10, 10, view->directionRay.x, view->directionRay.y, -20, 1.0f, 1.0f, 1.0f);
	glPopMatrix();
}
//--------------------------------------------------
void SimViewStateEP::handleMouseLeftPress(glAtrium* view, QMouseEvent *event)
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
		view->linkToMesh->m_stimulationID = view->itemAt(view->directionRay.x, view->directionRay.y, view->directionRay.z);
		view->linkToMachine->m_definitions->m_ectopicActivity = true;
	}	
}
void SimViewStateEP::handleMouseRightPress(glAtrium* view, QMouseEvent *event)
{
	view->setLastPos(event->pos());
	_quatStart = _quat;
}
void SimViewStateEP::handleMouseRelease(glAtrium* view, QMouseEvent *event)
{
	view->setLastPos(event->pos());

	//if (Qt::LeftButton)
	//{
		view->linkToMachine->m_definitions->m_ectopicActivity = false;

	//	glGetFloatv(GL_MODELVIEW_MATRIX, view->m);
	//	view->modelMatrix = view->m;
	//	int meshSize = view->linkToMesh->m_mesh.size();
	//	for (unsigned int j = 0; j < meshSize; ++j)
	//	{
	//		view->pointRays[j] = view->modelMatrix * Vector4(view->linkToMesh->m_mesh[j]->m_x, view->linkToMesh->m_mesh[j]->m_y, view->linkToMesh->m_mesh[j]->m_z, 1.0);
	//	}
	////}
}
void SimViewStateEP::handleMousewheel(glAtrium* view, QWheelEvent *event)
{
	double numDegrees = static_cast<double>(event->delta()) / 350.0;


	view->distanceToCamera -= view->zoomingSpeed*numDegrees;

	paintCursor(view);


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

}
void SimViewStateEP::handleMouseMove(glAtrium* view, QMouseEvent *event)
{
	double 	y = event->pos().y();
	double 	x = event->pos().x();
	double height = static_cast<double>(view->height());
	double width = static_cast<double>(view->width());

	view->directionRay = view->screenToWorld(x, y, width, height);

	if ((event->buttons() & Qt::LeftButton))
	{
		//view->setLastPos(event->pos());

		//double 	y = event->pos().y();
		//double 	x = event->pos().x();
		//double height = static_cast<double>(view->height());
		//double width = static_cast<double>(view->width());

		//view->directionRay = view->screenToWorld(x, y, width, height);

		//int item = view->itemAt(view->directionRay.x, view->directionRay.y, view->directionRay.z);
		//if (item != -1)
		//{
		//	view->testProbe.x = view->linkToMesh->m_mesh[item]->m_x;
		//	view->testProbe.y = view->linkToMesh->m_mesh[item]->m_y;
		//	view->testProbe.z = view->linkToMesh->m_mesh[item]->m_z;
		//	view->linkToMachine->m_stimulationID = view->itemAt(view->directionRay.x, view->directionRay.y, view->directionRay.z);
		//	view->linkToMachine->m_definitions->m_ectopicActivity = true;
		//}
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

		//GLfloat t[16];
		//glGetFloatv(GL_MODELVIEW_MATRIX, t);
		//view->modelRotation = t;
		//view->modelRotation = view->modelRotation* rotation;




		//view->setXRotation(view->xRot + view->rotationSpeed * (diff.y));
		//view->setYRotation(view->yRot + view->rotationSpeed * (diff.x));
		//	setZRotation(zRot + rotationSpeed * (next.y - previous.y));

		//setYRotation(yRot + rotationSpeed * (next.x - previous.x));
		//setZRotation(zRot + rotationSpeed * (next.y - previous.y));
		//setXRotation(xRot + rotationSpeed * (next.z - previous.z));
		//	float rotX = -1 * GLKMathDegreesToRadians(diff.y / 2.0);



		//Vector3 next = view->screenToWorld(event->x(), event->y(), view->viewWidth, view->viewHeight);
		//Vector3 previous = view->screenToWorld(view->lastPos.x(), view->lastPos.y(), view->viewWidth, view->viewHeight);

		//next
		//Vector3 diff = next - previous;

		//view->setXRotation(view->xRot + view->rotationSpeed * (diff.y));
		//view->setYRotation(view->yRot + view->rotationSpeed * (diff.x));
		////	setZRotation(zRot + rotationSpeed * (next.y - previous.y));

		////setYRotation(yRot + rotationSpeed * (next.x - previous.x));
		////setZRotation(zRot + rotationSpeed * (next.y - previous.y));
		////setXRotation(xRot + rotationSpeed * (next.z - previous.z));
		////	float rotX = -1 * GLKMathDegreesToRadians(diff.y / 2.0);
		//	float rotY = -1 * GLKMathDegreesToRadians(diff.x / 2.0);

		//view->lastPos = event->pos();
	}
	view->updateGL();
}