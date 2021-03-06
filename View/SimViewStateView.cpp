#include "View\SimViewStateView.h"
#include "View\glAtrium.h"
#include <boost\lexical_cast.hpp>
SimViewStateView* SimViewStateView::_instance = nullptr;

SimViewStateView::SimViewStateView()
{
	setPalette(DP_HOTTOCOLD);


}
SimViewStateView::~SimViewStateView()
{
}
//--------------------------------------------------
SimViewState* SimViewStateView::Instance()
{
	if (SimViewStateView::_instance == nullptr)
	{
		_instance = new SimViewStateView();
	}

	return _instance;
}
//--------------------------------------------------
//--------------------------------------------------

void SimViewStateView::setOutlineStyle(const  BRUSH_OUTLINE outline)
{

}

void SimViewStateView::paintCursor(glAtrium* view)
{
	int crossSize = 5;
	int sizepix = floor(abs(cursorRadius*view->height() * view->frustrumSize * view->nearClippingPlaneDistance / (view->distanceToCamera)));

	int pixmapsize;
	sizepix < 10 ? pixmapsize = 62 : pixmapsize = sizepix + 2;
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

	//painter.setBrush(QColor(256, 256, 256, 256));
	//char buff[20];
	//sprintf(buff, "%1.1f", cursorRadius * 2);
	//std::string str(buff);
	//painter.drawText(floor(3 + sizepix / 2.0), floor(-2 + sizepix / 2.0), QString(str.c_str()) + "mm");

	view->setCursor(QCursor(*m_LPixmap));
}
//--------------------------------------------------

//--------------------------------------------------
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
		view->linkToMachine->stimulator->setStimulationSiteID(view->itemAt(view->directionRay.x, view->directionRay.y, view->directionRay.z));
		view->linkToMachine->stimulator->start(view->linkToMesh);
	}	
}
void SimViewStateView::handleMouseRightPress(glAtrium* view, QMouseEvent *event)
{
	view->setLastPos(event->pos());
	_quatStart = _quat;
}
void SimViewStateView::handleMouseRelease(glAtrium* view, QMouseEvent *event)
{
	view->setLastPos(event->pos());

	view->linkToMachine->stimulator->stop(view->linkToMesh);
	view->linkToMachine->stimulator->setStimulationSiteID(view->linkToMachine->stimulator->probeElectrodeID(0));
	//if (Qt::LeftButton)
	//{
	//	glGetFloatv(GL_MODELVIEW_MATRIX, view->m);
	//	view->modelMatrix = view->m;
	//	int meshSize = view->linkToMesh->m_mesh.size();
	//	for (unsigned int j = 0; j < meshSize; ++j)
	//	{
	//		view->pointRays[j] = view->modelMatrix * Vector4(view->linkToMesh->m_mesh[j]->m_x, view->linkToMesh->m_mesh[j]->m_y, view->linkToMesh->m_mesh[j]->m_z, 1.0);
	//	}
	////}
}
void SimViewStateView::handleMousewheel(glAtrium* view, QWheelEvent *event)
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

		next = get_arcball_vector(view->width(), view->height(), event->x(), event->y());
		previous = get_arcball_vector(view->width(),view->height(), view->lastPos.x(), view->lastPos.y());

		computeIncremental(view, previous, next);

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
