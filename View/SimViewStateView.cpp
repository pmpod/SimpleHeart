#include "View\SimViewStateView.h"
#include "glAtrium.h"

SimViewStateView* SimViewStateView::_instance = nullptr;

SimViewStateView::SimViewStateView()
{
	cursorRadius = 2.0;
	_dataDisplayMode = 1;//POTENTIAL
	//_dataDisplayMode = 2;//CSD
}


SimViewStateView::~SimViewStateView()
{
}

void SimViewStateView::setDisplayMode(const int mode)
{
	_dataDisplayMode = mode;
}
SimViewState* SimViewStateView::Instance()
{
	if (SimViewStateView::_instance == nullptr)
	{
		_instance = new SimViewStateView();
	}
	//_instance->paintCursor(view, _instance->cursorRadius);

	return _instance;
}


void SimViewStateView::paintModel(glAtrium* view)
{
	glPushMatrix();

	// [1] Paint data

	CardiacMesh *msh = view->linkToMesh;
	std::vector<Oscillator*> oscs = view->linkToMesh->m_mesh;

	int vertexNumber = msh->m_vertexList.size();

	int t_ID1, t_ID2, t_ID3;
	GLfloat val1, val2, val3;
	GLfloat vmin = msh->m_minElectrogram;
	GLfloat vmax = msh->m_maxElectrogram;

	glBegin(GL_TRIANGLES);
	for (unsigned int j = 0; j < vertexNumber; j = j + 1)
	{
		t_ID1 = msh->m_vertexList[j]->id_1;
		t_ID2 = msh->m_vertexList[j]->id_2;
		t_ID3 = msh->m_vertexList[j]->id_3;

		if (oscs[t_ID1]->m_type != SOLID_WALL)
		{

			switch (_dataDisplayMode)
			{
			case 1:
				val1 = oscs[t_ID1]->m_v_scaledPotential;
				val2 = oscs[t_ID2]->m_v_scaledPotential;
				val3 = oscs[t_ID3]->m_v_scaledPotential;
				break;
			case 2:
				val1 = oscs[t_ID1]->getLastCurrentSource();
				val2 = oscs[t_ID2]->getLastCurrentSource();
				val3 = oscs[t_ID3]->getLastCurrentSource();
				break;
			case 3:
				val1 = oscs[t_ID1]->m_v_current[0];
				val2 = oscs[t_ID2]->m_v_current[0];
				val3 = oscs[t_ID3]->m_v_current[0];
				break;
			case 4:
				val1 = oscs[t_ID1]->m_v_current[1];
				val2 = oscs[t_ID2]->m_v_current[1];
				val3 = oscs[t_ID3]->m_v_current[1];
				break;
			}

			paintCellTriangle(oscs[t_ID1]->m_x, oscs[t_ID1]->m_y,
				oscs[t_ID1]->m_z, val1,
				oscs[t_ID2]->m_x, oscs[t_ID2]->m_y,
				oscs[t_ID2]->m_z, val2,
				oscs[t_ID3]->m_x, oscs[t_ID3]->m_y,
				oscs[t_ID3]->m_z, val3,
				view->m_palette, vmin, vmax);
		}
		else
		{
			paintCellTriangle(oscs[t_ID1]->m_x,
				oscs[t_ID1]->m_y, oscs[t_ID1]->m_z, 0.0,
				oscs[t_ID2]->m_x, oscs[t_ID2]->m_y, oscs[t_ID2]->m_z, 0.0,
				oscs[t_ID3]->m_x, oscs[t_ID3]->m_y, oscs[t_ID3]->m_z, 0.0,
				3, 0, 1);
		}

	}
	glEnd();








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
	glPopMatrix();
}

void  SimViewStateView::paintCursor(glAtrium* view)
{
	int crossSize = 5;
	int sizepix = floor(abs(cursorRadius*view->height() * view->frustrumSize * view->nearClippingPlaneDistance / (view->distanceToCamera)));
	QPixmap* m_LPixmap = new QPixmap(sizepix + 2, sizepix + 2);
	m_LPixmap->fill(Qt::transparent);
	QPainter painter(m_LPixmap);
	QColor col(0, 0, 64, 128);

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
void SimViewStateView::handleMouseLeftPress(glAtrium* view, QMouseEvent *event)
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
}
void SimViewStateView::handleMouseRightPress(glAtrium* view, QMouseEvent *event)
{
	view->setLastPos(event->pos());
}
void SimViewStateView::handleMouseRelease(glAtrium* view, QMouseEvent *event)
{
	view->setLastPos(event->pos());

	if (Qt::LeftButton)
	{
		view->linkToMachine->m_definitions->m_ectopicActivity = false;

		glGetFloatv(GL_MODELVIEW_MATRIX, view->m);
		view->modelMatrix = view->m;
		int meshSize = view->linkToMesh->m_mesh.size();
		for (unsigned int j = 0; j < meshSize; ++j)
		{
			view->pointRays[j] = view->modelMatrix * Vector4(view->linkToMesh->m_mesh[j]->m_x, view->linkToMesh->m_mesh[j]->m_y, view->linkToMesh->m_mesh[j]->m_z, 1.0);
		}
	}
}
void SimViewStateView::handleMousewheel(glAtrium* view, QWheelEvent *event)
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
void SimViewStateView::handleMouseMove(glAtrium* view, QMouseEvent *event)
{
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
		Vector3 diff = next - previous;

		view->setXRotation(view->xRot + view->rotationSpeed * (diff.y));
		view->setYRotation(view->yRot + view->rotationSpeed * (diff.x));
		//	setZRotation(zRot + rotationSpeed * (next.y - previous.y));

		//setYRotation(yRot + rotationSpeed * (next.x - previous.x));
		//setZRotation(zRot + rotationSpeed * (next.y - previous.y));
		//setXRotation(xRot + rotationSpeed * (next.z - previous.z));
		//	float rotX = -1 * GLKMathDegreesToRadians(diff.y / 2.0);
		//	float rotY = -1 * GLKMathDegreesToRadians(diff.x / 2.0);

		view->lastPos = event->pos();
	}
}