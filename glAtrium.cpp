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
	farClippingPlaneDistance = 500.0;


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
	_state = SimViewStateView::Instance();
}
//----------------------------------------
glAtrium::~glAtrium(void)
{

	glDeleteLists(displayListIndex, 1);
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

void  glAtrium::setDisplayPotential(bool b)
{
	if (b)
		SimViewStateView::Instance()->setDisplayMode(1);
	updateGL();
}
void  glAtrium::setDisplayCSD(bool b)
{
	if (b)
		SimViewStateView::Instance()->setDisplayMode(2);
	updateGL();
}
void  glAtrium::setDisplayCurrent1(bool b)
{
	if (b)
		SimViewStateView::Instance()->setDisplayMode(3);
	updateGL();
}
void  glAtrium::setDisplayCurrent2(bool b)
{
	if (b)
		SimViewStateView::Instance()->setDisplayMode(4);
	updateGL();
}
void glAtrium::displayElectrogram()
{
	SimViewStateView::Instance()->setDisplayMode(5);
	updateGL();
}

//----------------------------------------
void glAtrium::setStateStructureModifier(bool b)
{
	if (b)
		ChangeState(SimViewStateStructure::Instance());
	_state->paintCursor(this);
	_state->prepareLegend(this);
	updateGL();
}
//----------------------------------------
void glAtrium::setStateViewer(bool b)
{
	if (b)
		ChangeState(SimViewStateView::Instance());
	_state->paintCursor(this);
	_state->prepareLegend(this);
	updateGL();
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

	glShadeModel(GL_FLAT);						// Enables Smooth Shading
	glShadeModel(GL_SMOOTH);						// Enables Smooth Shading
	glClearDepth(1.0f);							// Depth Buffer Setup
	//glDisable(GL_DEPTH_TEST);						// Enables Depth Testing
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

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);


	for (unsigned int j = 0; j < linkToMesh->m_mesh.size(); ++j)
	{
		pointRays.push_back(Vector4( linkToMesh->m_mesh[j]->m_x,
									 linkToMesh->m_mesh[j]->m_y,
							 		 linkToMesh->m_mesh[j]->m_z, 
									 1.0) );
	}
	displayListIndex = glGenLists(1);
	_state->prepareLegend(this);
}
//----------------------------------------

//--------------------------------------------
void glAtrium::paintOrigin(float scale)
{
	float f[1];
	GLfloat length = frustrumSize*scale;
	GLfloat col1[] = { 1.0, 0.0, 0.0, 1.f };
	GLfloat col2[] = { 0.0, 1.0, 0.0, 1.f };
	GLfloat col3[] = { 0.0, 0.0, 1.0, 1.f };
	GLfloat col_w[] = { 1.0, 1.0, 1.0, 1.f };
	glGetFloatv(GL_LINE_WIDTH, f);
	glLineWidth(1.0f);
	glBegin(GL_LINES);
		//glMaterialfv(GL_FRONT, GL_AMBIENT, col1);

		glColor3fv(col1);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(length, 0.0f, 0.0f);
		glColor3fv(col2);
		//glMaterialfv(GL_FRONT, GL_AMBIENT, col2);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, -length, 0.0f);
		glColor3fv(col3);
		//glMaterialfv(GL_FRONT, GL_AMBIENT, col3);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, length);
	glEnd();
	glColor3fv(col_w);
	renderText(length, 0.0f, 0.0f, "x");
	renderText( 0.0f, -length, 0.0f, "y");
	renderText(0.0f, 0.0f, length, "z");
	glLineWidth(f[0]);
}
void glAtrium::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	qglClearColor(backgroundColor);
	glLoadIdentity();

	//[1] Paint origin
		glTranslatef(	-1.1f*frustrumSize*(static_cast<double>(this->width()) / static_cast<double>(this->height())), 
						1.1f*frustrumSize, 
						-nearClippingPlaneDistance-0.5f);
		glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
		glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
		glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
		paintOrigin(0.1f);


	//[1] Paint legend
		glLoadIdentity();
		glTranslatef(1.0f*frustrumSize*(static_cast<double>(this->width()) / static_cast<double>(this->height())),
			0*frustrumSize,
			-nearClippingPlaneDistance - 0.5f);



		_state->paintLegend(this);

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



		_state->paintModel(this);

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
		int meshSize = linkToMesh->m_mesh.size();
		for (unsigned int j = 0; j < meshSize; ++j)
		{
			pointRays[j] = modelMatrix * Vector4(linkToMesh->m_mesh[j]->m_x, linkToMesh->m_mesh[j]->m_y, linkToMesh->m_mesh[j]->m_z, 1.0);
		}
		_state->paintCursor(this);
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

	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	modelMatrix = m;
	int meshSize = linkToMesh->m_mesh.size();
	for (unsigned int j = 0; j < meshSize; ++j)
	{
		pointRays[j] = modelMatrix * Vector4(linkToMesh->m_mesh[j]->m_x, linkToMesh->m_mesh[j]->m_y, linkToMesh->m_mesh[j]->m_z, 1.0);
		
		Vector3 rayTested(pointRays[j].x, pointRays[j].y, pointRays[j].z);
		templ = pointRays[j].length();
		temp = (rayTested.normalize()).cross(rayClicked.normalize()).length();

		if (temp <0.007)
		{
			if (templ < length)
			{
				epsilon = temp;
				index = j;
				length = templ;
				break;
			}
		}

	}
	return index;
}
//----------------------------------------
void glAtrium::wheelEvent(QWheelEvent *event)
{
	_state->handleMousewheel(this, event);
	updateGL();
}

void glAtrium::mousePressEvent(QMouseEvent *event)
{

	if (event->button() == Qt::LeftButton)
	{
		_state->handleMouseLeftPress(this, event);
	}
	else if((event->button() == Qt::RightButton))
	{
		_state->handleMouseRightPress(this, event);
	}
	updateGL();
}

void glAtrium::mouseReleaseEvent(QMouseEvent *event)
{
	_state->handleMouseRelease(this, event);
	updateGL();
}
//----------------------------------------
void glAtrium::mouseMoveEvent(QMouseEvent *event)
{
	_state->handleMouseMove(this, event);
}
void glAtrium::setLastPos(const QPoint& pos)
{
	lastPos = pos;
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
void glAtrium::ChangeState(SimViewState* state)
{
	_state = state;
}