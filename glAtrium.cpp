#include "glAtrium.h"
#include <QtGui>
#include <QtOpenGL>

#include <cmath>

double round(double fValue)
{
	return fValue < 0 ? ceil(fValue - 0.5)
		: floor(fValue + 0.5);
}
#define ELECTROGRAMMM 1
glAtrium::glAtrium(CardiacMesh *linkMesh, AtrialMachine2d *link, QWidget *parent)
    : QGLWidget(parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
	rotationSpeed = 500.0f;
	zoomingSpeed = 10.0f;
	m_palette = 0;

	backgroundColor = QColor(50, 50, 50);

    linkToMachine = link;
	linkToMesh = linkMesh;

	workMode=3;
	
	m_controlable = false;
	paintRay = false;
	frustrumSize = 1.0f;
	nearClippingPlaneDistance = 2.0;
	farClippingPlaneDistance = 80.0;


	LightAmbient[0] = 1.0f;
	LightAmbient[1] = 1.0f;
	LightAmbient[2] = 1.0f;
	LightAmbient[3] = 1.0f;

	LightDiffuse[0] = 1.0f;
	LightDiffuse[1] = 1.0f;
	LightDiffuse[2] = 1.0f;
	LightDiffuse[3] = 1.0f;

	LightPosition[0] = 0.0f;
	LightPosition[1] = 0.0f;
	LightPosition[2] = 0.0f;
	LightPosition[3] = 1.0f;

	distanceToCamera = -40.0f;
}
//----------------------------------------
glAtrium::~glAtrium(void)
{
	makeCurrent();
}
//----------------------------------------
QSize glAtrium::minimumSizeHint() const
{
    return QSize(50, 50);
}
//----------------------------------------
void  glAtrium::setPaletteHSV()
{
	m_palette = 0;
}
//----------------------------------------
void  glAtrium::setPaletteGray()
{
	m_palette = 1;
}
//----------------------------------------
QSize glAtrium::sizeHint() const
{
	return QSize(this->width(), this->height());
}
//----------------------------------------
void glAtrium::setXRotation(int angle)
{
    normalizeAngle(angle);
    if (angle != xRot) 
	{
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}
//----------------------------------------
void glAtrium::setYRotation(int angle)
{
    normalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}
//----------------------------------------
void glAtrium::setZRotation(int angle)
{
    normalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}
void glAtrium::setTop()
{
	setXRotation(0);
	setYRotation(0);
	setZRotation(0);
}
void glAtrium::setSide1()
{
	setXRotation(700);
	setYRotation(0);
	setZRotation(350);
}
void glAtrium::setSide2()
{
	setXRotation(1000);
	setYRotation(0);
	setZRotation(-650);
}
//----------------------------------------
void glAtrium::initializeGL()
{
	viewHeight = static_cast<double>(this->height());
	viewWidth = static_cast<double>(this->width());

	glShadeModel(GL_SMOOTH);						// Enables Smooth Shading
	glClearDepth(1.0f);							// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);							// The Type Of Depth Test To Do


	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_LIGHTING);
		glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, LightPosition);
		glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.01f);
		glEnable(GL_LIGHT1);



	for (unsigned int j = 0; j < linkToMesh->m_mesh.size(); ++j)
	{
		pointRays.push_back( Vector4(linkToMesh->m_mesh[j]->getPositionX(),
							 		 linkToMesh->m_mesh[j]->getPositionY(),
							 		 linkToMesh->m_mesh[j]->getPositionZ(), 
									 1.0) );
	}

}
//----------------------------------------
void glAtrium::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	qglClearColor(backgroundColor);

	//[1] Paint origin
	glLoadIdentity();
		glTranslatef(-1.1f*frustrumSize*(static_cast<double>(this->width()) / static_cast<double>(this->height())), 1.1f*frustrumSize, -nearClippingPlaneDistance-0.5f);
		glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
		glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
		glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
		paintOrigin(frustrumSize, 0.1f);

	//[2] Paint the clicking ray
	if (paintRay)
	{
		glLoadIdentity();
		paintTheRay(directionRay.x, directionRay.y, directionRay.z);
		//for (unsigned int j = 0; j < pointRays.size(); ++j)
		//{
			//paintTheRay(pointRays[j].x , pointRays[j].y , pointRays[j].z );
			//drawSphere(0.2, 10, 10, pointRays[j].x, pointRays[j].y, pointRays[j].z);
		//}
	}

	//[2] Paint the model
	glLoadIdentity();
		glTranslatef(0, 0, distanceToCamera);
		glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
		glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
		glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
		glTranslatef(-linkToMesh->centerGeom.x, -linkToMesh->centerGeom.y, linkToMesh->centerGeom.z);

	glPushMatrix();

	int vertexNumber = linkToMesh->m_vertexList.size();
	for (unsigned int j = 0; j < vertexNumber; ++j)
	{
		//if (linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_type != SOLID_WALL)
		//{
			paintCellTriangle(linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_x,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_y,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_z, linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->getElectrogram(),
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_2]->m_x,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_2]->m_y,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_2]->m_z, linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_2]->getElectrogram(),
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_3]->m_x,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_3]->m_y,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_3]->m_z, linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_3]->getElectrogram(),
				m_palette);
		//}
		//else
		//{
		//	paintCellTriangle(linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_x,
		//		linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_y,
		//		linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_z, 0.0,
		//		linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_2]->m_x,
		//		linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_2]->m_y,
		//		linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_2]->m_z, 0.0,
		//		linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_3]->m_x,
		//		linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_3]->m_y,
		//		linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_3]->m_z, 0,
		//		m_palette);
		////}

	}
	// [3] Paint probes
	for (short n = 0; n < linkToMachine->probeOscillator.size(); ++n)
	{
		drawSphere(0.2, 10, 10, linkToMachine->probeOscillator[n]->getPositionX(),
			linkToMachine->probeOscillator[n]->getPositionY(),
			linkToMachine->probeOscillator[n]->getPositionZ(), 1.0f / (n + 1), 1.0f / (n + 1), 1.0f);
	}
	if (paintRay)
	{
		drawSphere(0.2, 10, 10, testProbe.x, testProbe.y, testProbe.z, 1.0f, 1.0f, 1.0f);
	}
	glPopMatrix();
}
//----------------------------------------
void glAtrium::resizeGL(int width, int height)
{ 
	viewHeight = width;
	viewWidth = height;

	makeCurrent();

	//[1] Set projection view parameters
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(	-frustrumSize*(static_cast<double>(width) / static_cast<double>(height)),
					+frustrumSize*(static_cast<double>(width) / static_cast<double>(height)),
					+frustrumSize, 
					-frustrumSize, nearClippingPlaneDistance, farClippingPlaneDistance);


	//[2] Set model view raycasting parameters
	glMatrixMode(GL_MODELVIEW);
		glGetFloatv(GL_MODELVIEW_MATRIX, m);
		modelMatrix = m;
		for (unsigned int j = 0; j < linkToMesh->m_mesh.size(); ++j)
		{
			pointRays[j] = modelMatrix * Vector4(	linkToMesh->m_mesh[j]->getPositionX(),
													linkToMesh->m_mesh[j]->getPositionY(),
													linkToMesh->m_mesh[j]->getPositionZ(),
													1.0);
		}
}
//----------------------------------------
int glAtrium::itemAt(double xx, double yy, double zz)
{
	double length = 100;
	double epsilon = 0;
	double index = -1;

	double temp = 0;
	double templ = 0;

	Vector3 rayClicked(xx, yy, zz);

	for (unsigned int j = 0; j < linkToMesh->m_mesh.size(); ++j)
	{
		Vector3 rayTested(pointRays[j].x, pointRays[j].y, pointRays[j].z);
		templ = rayTested.length();
		temp = (rayTested.normalize()).cross(rayClicked.normalize()).length();

		if (temp <0.007)
		{
			if (templ < length)
			{
				epsilon = temp;
				index = j;
				length = templ;
			}
		}

	}
	return index;
}
//----------------------------------------
void glAtrium::wheelEvent(QWheelEvent *event)
{
	double numDegrees = static_cast<double>(event->delta()) / 350.0;


	distanceToCamera -= zoomingSpeed*numDegrees;
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
	updateGL();
}

void glAtrium::mousePressEvent(QMouseEvent *event)
{
	if (m_controlable)
	{
		lastPos = event->pos();
	}
	if (event->button() == Qt::LeftButton)
	{
		double 	y = event->pos().y();
		double 	x = event->pos().x();
		double height = static_cast<double>(this->height());
		double width = static_cast<double>(this->width());

		directionRay = screenToWorld(x, y, width, height);

		int item = itemAt(directionRay.x, directionRay.y, directionRay.z);
		if (item != -1)
		{
			testProbe.x = linkToMesh->m_mesh[itemAt(directionRay.x, directionRay.y, directionRay.z)]->m_x;
			testProbe.y = linkToMesh->m_mesh[itemAt(directionRay.x, directionRay.y, directionRay.z)]->m_y;
			testProbe.z = linkToMesh->m_mesh[itemAt(directionRay.x, directionRay.y, directionRay.z)]->m_z;
			linkToMachine->m_stimulationID = itemAt(directionRay.x, directionRay.y, directionRay.z);
			linkToMachine->m_definitions->m_ectopicActivity = true;
		}

		//if (workMode == 7)
		//{
		//	linkToMesh->m_mesh[itemAt(pos.x, pos.y, pos.z)]->m_type = SOLID_WALL;
		//	for (int ss = 0; ss < linkToMesh->m_mesh[itemAt(pos.x, pos.y, pos.z)]->m_neighbours.size(); ss++)
		//	{
		//		linkToMesh->m_mesh[linkToMesh->m_mesh[itemAt(pos.x, pos.y, pos.z)]->m_neighbours[ss]->oscillatorID]->m_type = SOLID_WALL;
		//	}
		//	linkToMesh->setWallCells();
		//}

		//else if (workMode == 8)
		//{
		//	linkToMesh->m_mesh[itemAt(pos.x, pos.y, pos.z)]->m_type = ATRIAL_V3;
		//	for (int ss = 0; ss < linkToMesh->m_mesh[itemAt(pos.x, pos.y, pos.z)]->m_neighbours.size(); ss++)
		//	{
		//		linkToMesh->m_mesh[linkToMesh->m_mesh[itemAt(pos.x, pos.y, pos.z)]->m_neighbours[ss].first]->m_type = ATRIAL_V3;
		//	}
		//	m_mesh->setWallCells();
		//}
		updateGL();
	}
}

void glAtrium::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_controlable)
	{
		lastPos = event->pos();
	}

	linkToMachine->m_definitions->m_ectopicActivity = false;

	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	modelMatrix = m;
	for (unsigned int j = 0; j < linkToMesh->m_mesh.size(); ++j)
	{
		pointRays[j] = modelMatrix * Vector4(linkToMesh->m_mesh[j]->m_x, linkToMesh->m_mesh[j]->m_y, linkToMesh->m_mesh[j]->m_z, 1.0);
	}
	updateGL();
}
//----------------------------------------
void glAtrium::mouseMoveEvent(QMouseEvent *event)
{
	if (m_controlable && (event->buttons() & Qt::RightButton))
	{

		Vector3 next = screenToWorld(event->x(), event->y(), viewWidth, viewHeight);
		Vector3 previous = screenToWorld(lastPos.x(), lastPos.y(), viewWidth, viewHeight);
		Vector3 diff = next - previous;

		setXRotation(xRot + rotationSpeed * (diff.y));
		setYRotation(yRot + rotationSpeed * (diff.x));
	//	setZRotation(zRot + rotationSpeed * (next.y - previous.y));

		//setYRotation(yRot + rotationSpeed * (next.x - previous.x));
		//setZRotation(zRot + rotationSpeed * (next.y - previous.y));
		//setXRotation(xRot + rotationSpeed * (next.z - previous.z));
	//	float rotX = -1 * GLKMathDegreesToRadians(diff.y / 2.0);
	//	float rotY = -1 * GLKMathDegreesToRadians(diff.x / 2.0);

		lastPos = event->pos();
	}
}
//------------------------------------------
Vector3 glAtrium::screenToWorld(double x, double y, double width, double height)
{
	Vector3 pos;
	Vector3 view;

	double vLength = frustrumSize;
	double hLength = vLength*width / height;

	view.x = 0.0;
	view.y = 0.0;
	view.z = -1.0f;

	Vector3 v;
	v.x = 0.0f;
	v.y = vLength;
	v.z = 0.0;

	Vector3 h;
	h.x = hLength;
	h.y = 0.0f;
	h.z = 0.0;

	// translate mouse coordinates so that the origin lies in the center of the view port
	y -= (height / 2);
	x -= (width / 2);
	y = y / ((height / 2));
	x = x / (width / 2);

	pos.x = hLength*x;
	pos.y = vLength*y;
	pos.z = view.z*nearClippingPlaneDistance;
	//pos.x = 0 + view.x*nearClippingPlaneDistance + h.x*x + v.x*y;
	//pos.y = 0 + view.y*nearClippingPlaneDistance + h.y*x + v.y*y;
	//pos.z = 0 + view.z*nearClippingPlaneDistance + h.z*x + v.z*y;

	return pos;

}