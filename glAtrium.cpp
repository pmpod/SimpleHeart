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

	_state = SimViewStateStructure::Instance(this);
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
void glAtrium::setStateStructureModifier(bool b)
{
	if (b)
		ChangeState(SimViewStateStructure::Instance(this));
}
//----------------------------------------
void glAtrium::setStateViewer(bool b)
{
	if (b)
		ChangeState(SimViewStateView::Instance(this));
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



	for (unsigned int j = 0; j < linkToMesh->m_mesh.size(); ++j)
	{
		pointRays.push_back( Vector4(linkToMesh->m_mesh[j]->getPositionX(),
							 		 linkToMesh->m_mesh[j]->getPositionY(),
							 		 linkToMesh->m_mesh[j]->getPositionZ(), 
									 1.0) );
	}

}
//----------------------------------------
void glAtrium::paintLegend(float scale)
{
	float f[1];
	GLfloat widthL = frustrumSize*scale*0.5;
	GLfloat heightL = frustrumSize*scale*0.2;

	GLfloat col_w[] = { 1.0, 1.0, 1.0, 1.f };
	GLfloat col_g[] = { .5f, .5f, .5f, .5f };
	GLfloat col[] = { 0.0, 0.0, 1.0, 1.f };

	GLfloat ccol;
	glBegin(GL_QUAD_STRIP);
	for (GLfloat i = -16.0; i <= 16.0; ++i)
	{
		ccol = -i;
		hotToColdMap(ccol, -16.0, 16.0, col[0], col[1], col[2]);
		glMaterialfv(GL_FRONT, GL_AMBIENT, col);
		glVertex3f(widthL, i*heightL, 0.0f);
		glVertex3f(-widthL, i*heightL, 0.0f);

	}
	glEnd();
	glBegin(GL_LINES);
	glMaterialfv(GL_FRONT, GL_AMBIENT, col_g);
	for (GLfloat i = -15.0; i <= 15.0; ++i)
	{
		glVertex3f(-widthL, i*heightL, 0.0f);
		glVertex3f(widthL, i*heightL, 0.0f);

	}
	glEnd();

	glBegin(GL_LINES);
	glMaterialfv(GL_FRONT, GL_AMBIENT, col_w);
		glVertex3f(-widthL, -16.0*heightL, 0.0f);
		glVertex3f(widthL*1.2f, -16.0*heightL, 0.0f);
		glVertex3f(-widthL, 0.0*heightL, 0.0f);
		glVertex3f(widthL*1.2f, 0.0*heightL, 0.0f);
		glVertex3f(-widthL, 16.0*heightL, 0.0f);
		glVertex3f(widthL*1.2f, 16.0*heightL, 0.0f);
	glEnd();

	glMaterialfv(GL_FRONT, GL_AMBIENT, col_w);
	renderText(widthL*1.4f, 16.0*heightL - heightL/2, 0, "-75 mV");
	renderText(widthL*1.4f, -16.0*heightL - heightL / 2, 0, "25 mV");
	renderText(widthL*1.4f, 0.0*heightL - heightL / 2, 0, "0");

}
//--------------------------------------------
void glAtrium::paintOrigin(float scale)
{
	float f[1];
	GLfloat length = frustrumSize*scale;
	GLfloat col1[] = { 1.0, 0.0, 0.0, 1.f };
	GLfloat col2[] = { 0.0, 1.0, 0.0, 1.f };
	GLfloat col3[] = { 0.0, 0.0, 1.0, 1.f };
	glGetFloatv(GL_LINE_WIDTH, f);
	glLineWidth(1.0f);
	glBegin(GL_LINES);
		glMaterialfv(GL_FRONT, GL_AMBIENT, col1);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(length, 0.0f, 0.0f);
		glMaterialfv(GL_FRONT, GL_AMBIENT, col2);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, -length, 0.0f);
		glMaterialfv(GL_FRONT, GL_AMBIENT, col3);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, length);
	glEnd();
	renderText(length, 0.0f, 0.0f, "x");
	renderText( 0.0f, -length, 0.0f, "y");
	renderText(0.0f, 0.0f, length, "z");
	glLineWidth(f[0]);
}
void glAtrium::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	qglClearColor(backgroundColor);

	//[1] Paint origin
	glLoadIdentity();
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
		paintLegend(0.1f);

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
	int t_ID1;
	int t_ID2;
	int t_ID3;

	GLfloat val;
	GLfloat color[] = { 0.f, 0.f, 0.f, 1.f };
	GLfloat xx, yy, zz;

	GLfloat vmin = linkToMesh->m_minElectrogram;
	GLfloat vmax = linkToMesh->m_maxElectrogram;

	for (unsigned int j = 0; j < vertexNumber; j=j+1)
	{
		//if (linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_type != SOLID_WALL)
		//{
		t_ID1 = linkToMesh->m_vertexList[j]->id_1;
		t_ID2 = linkToMesh->m_vertexList[j]->id_2;
		t_ID3 = linkToMesh->m_vertexList[j]->id_3;

		glBegin(GL_TRIANGLES);



			////glNormal3d( (y2-y1)*(z3-z1) - (z2-z1)*(y3-y1), 
			////			(z2-z1)*(x3-x1) - (x2-x1)*(z3-z1), 
			////			(x2-x1)*(y3-y1) - (y2-y1)*(x3-x1) );
			//val = linkToMesh->m_mesh[t_ID1]->m_v_electrogram;
			//hotToColdMap(val, vmin, vmax, color[0], color[1], color[2]);
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
			//glMaterialfv(GL_FRONT, GL_AMBIENT, color);
			//glVertex3f(linkToMesh->m_mesh[t_ID1]->m_x, linkToMesh->m_mesh[t_ID1]->m_y, linkToMesh->m_mesh[t_ID1]->m_z);

			//val = linkToMesh->m_mesh[t_ID2]->m_v_electrogram;
			//hotToColdMap(val, vmin, vmax, color[0], color[1], color[2]);
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
			//glMaterialfv(GL_FRONT, GL_AMBIENT, color);
			//glVertex3f(linkToMesh->m_mesh[t_ID2]->m_x, linkToMesh->m_mesh[t_ID2]->m_y, linkToMesh->m_mesh[t_ID2]->m_z);


			//val = linkToMesh->m_mesh[t_ID3]->m_v_electrogram;
			//hotToColdMap(val, vmin, vmax, color[0], color[1], color[2]);
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
			//glMaterialfv(GL_FRONT, GL_AMBIENT, color);
			//glVertex3f(linkToMesh->m_mesh[t_ID3]->m_x, linkToMesh->m_mesh[t_ID3]->m_y, linkToMesh->m_mesh[t_ID3]->m_z);

			paintCellTriangle(linkToMesh->m_mesh[t_ID1]->m_x,
				linkToMesh->m_mesh[t_ID1]->m_y,
				linkToMesh->m_mesh[t_ID1]->m_z, linkToMesh->m_mesh[t_ID1]->m_v_electrogram,
				linkToMesh->m_mesh[t_ID2]->m_x,
				linkToMesh->m_mesh[t_ID2]->m_y,
				linkToMesh->m_mesh[t_ID2]->m_z, linkToMesh->m_mesh[t_ID2]->m_v_electrogram,
				linkToMesh->m_mesh[t_ID3]->m_x,
				linkToMesh->m_mesh[t_ID3]->m_y,
				linkToMesh->m_mesh[t_ID3]->m_z, linkToMesh->m_mesh[t_ID3]->m_v_electrogram,
				m_palette, linkToMesh->m_minElectrogram, linkToMesh->m_maxElectrogram);
		glEnd();
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

		renderText(linkToMachine->probeOscillator[n]->getPositionX(),
			linkToMachine->probeOscillator[n]->getPositionY(),
			linkToMachine->probeOscillator[n]->getPositionZ()+1, "Electrode");
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
		_state->paintCursor(this, _state->getRadius());
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