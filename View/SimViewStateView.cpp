#include "View\SimViewStateView.h"
#include "glAtrium.h"

SimViewStateView* SimViewStateView::_instance = nullptr;

SimViewStateView::SimViewStateView(glAtrium* view)
{
	cursorRadius = 2.0;
	_view = view;
}


SimViewStateView::~SimViewStateView()
{
}

SimViewState* SimViewStateView::Instance(glAtrium* view)
{
	if (SimViewStateView::_instance == nullptr)
	{
		_instance = new SimViewStateView(view);
	}
	_instance->paintCursor(view, _instance->cursorRadius);

	return _instance;
}


void  SimViewStateView::paintCursor(glAtrium* view, float radius)
{
	int crossSize = 5;
	int sizepix = floor(abs(radius*view->height() * view->frustrumSize * view->nearClippingPlaneDistance / (view->distanceToCamera)));
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