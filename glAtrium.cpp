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
glAtrium::glAtrium(CardiacMesh *linkMesh, AtrialMachine2d *link, DiffusionMatrix *pixelmap, QWidget *parent)
    : QGLWidget(parent)
{
	object = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;
	m_palette = 0;

	colorek =  QColor(50, 50, 50 );//parent->palette().color(QPalette::Base);//QColor(255, 255, 255 );
    qtGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    qtPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);

    linkToMachine = link;
	linkToMesh = linkMesh;

    m_pixelmap=pixelmap;

	m_controlable = true;
	workMode=3;
	
	pixelSize = 1.0f / static_cast<double>( link->m_grid->getSize());

	paintRay = true;
	frustrumSize = 1.0f;
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

	direction.x = 0.0;
	direction.y = 0.0;
	direction.z = 0.0;

	distanceToCamera = -40.0f;
	tempZoom = 1;
}
//----------------------------------------
glAtrium::~glAtrium(void)
{
	makeCurrent();
    glDeleteLists(object, 1);
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
	//return QSize(500, 500);
	return QSize(this->width(), this->height());
}
void glAtrium::changeColor(double value)
{
	value +=4;
	value *= 20;
	int cent = static_cast<int>(value);
	//cent %=255;
	colorek =  QColor(cent, cent, cent );
    updateGL();
}
//----------------------------------------
void glAtrium::setXRotation(int angle)
{
    normalizeAngle(&angle);
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
    normalizeAngle(&angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}
//----------------------------------------
void glAtrium::setZRotation(int angle)
{
    normalizeAngle(&angle);
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
	//makeCurrent();
    qglClearColor(QColor(50, 50, 50 ));
    object = makeObject();				///@TODO makeobject
    //glShadeModel(GL_FLAT);
	glShadeModel(GL_SMOOTH);						// Enables Smooth Shading
	glClearDepth(1.0f);							// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);							// The Type Of Depth Test To Do


	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
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

	resizeGL(this->width(), this->height());

	//////gridSize = linkToMachine->m_grid->m_net.size() - 1;
	////gridSize = linkToMachine->m_grid->getSize();
 ////  // glEnable(GL_DEPTH_TEST);
 ////   //glEnable(GL_CULL_FACE);
	////for (int j = 0; j < linkToMachine->m_grid->getSize(); ++j)
	////{
	////	for (int i = 0; i < linkToMachine->m_grid->getSize(); ++i)
	////	{
	////		double a = (static_cast<double>(this->width())) / static_cast<double>(linkToMachine->m_grid->getSize());
	////		double b = (static_cast<double>(this->height())) / static_cast<double>(linkToMachine->m_grid->getSize());
	////		m_pixelmap->m_matrix[j][i]->size = min( (static_cast<double>(this->width())) / static_cast<double>(linkToMachine->m_grid->getSize()), (static_cast<double>(this->height())) /static_cast<double>( linkToMachine->m_grid->getSize()));
	////	}

	////}
}
//----------------------------------------
void glAtrium::paintGL()
{
		//makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	qglClearColor(colorek);
	glLoadIdentity();

	glTranslatef(-1.1f*frustrumSize*(static_cast<double>(this->width()) / static_cast<double>(this->height())), 1.1f*frustrumSize, -nearClippingPlaneDistance-0.5f);
	glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
	glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
	glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
	paintOrigin(frustrumSize, 0.1f);

    glLoadIdentity();

	//Paint the clicking ray
	paintTheRay(direction.x, direction.y, direction.z);

	//paint rays to the model
	for (unsigned int j = 0; j < pointRays.size(); ++j)
	{
		//paintTheRay(pointRays[j].x , pointRays[j].y , pointRays[j].z );
		//drawSphere(0.2, 10, 10, pointRays[j].x, pointRays[j].y, pointRays[j].z);
	}

	glTranslatef(0, 0, distanceToCamera);
	glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
	glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
	glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
	glTranslatef(-linkToMesh->centerGeom.x, -linkToMesh->centerGeom.y, linkToMesh->centerGeom.z);

	glPushMatrix();

	for (unsigned int j = 0; j < linkToMesh->m_vertexList.size(); ++j)
	{
		if (linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_type != SOLID_WALL)
		{
			paintCellTriangle(linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_x*tempZoom,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_y*tempZoom,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_z*tempZoom, (0.3 + (linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->getElectrogram()) / 2.0),
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_2]->m_x*tempZoom,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_2]->m_y*tempZoom,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_2]->m_z*tempZoom, (0.3 + (linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_2]->getElectrogram()) / 2.0),
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_3]->m_x*tempZoom,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_3]->m_y*tempZoom,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_3]->m_z*tempZoom, (0.3 + (linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_3]->getElectrogram()) / 2.0),
				m_palette);
		}
		else
		{
			paintCellTriangle(linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_x*tempZoom,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_y*tempZoom,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_1]->m_z*tempZoom, 0,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_2]->m_x*tempZoom,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_2]->m_y*tempZoom,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_2]->m_z*tempZoom, 0,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_3]->m_x*tempZoom,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_3]->m_y*tempZoom,
				linkToMesh->m_mesh[linkToMesh->m_vertexList[j]->id_3]->m_z*tempZoom, 0,
				m_palette);
		}

	}
	//paintProbe(linkToMachine->probeOscillator[0]->getPositionX(),linkToMachine->probeOscillator[0]->getPositionY(),0.01f,0.01f, 1,1,0);
	//paintProbe(linkToMachine->probeOscillator[1]->getPositionX(),linkToMachine->probeOscillator[1]->getPositionY(),0.01f,0.01f, 1,0,0);
	//paintProbe(linkToMachine->probeOscillator[2]->getPositionX(),linkToMachine->probeOscillator[2]->getPositionY(),0.01f,0.01f, 0,1,0);
	//if(paintRay)
	//{
	//}
	drawSphere(0.2, 10, 10, testProbe.x, testProbe.y, testProbe.z);
	glPopMatrix();


//	//gluLookAt(objCamera.mPos.x,  objCamera.mPos.y,  objCamera.mPos.z,	
//	//	  objCamera.mView.x, objCamera.mView.y, objCamera.mView.z,	
//	//		  objCamera.mUp.x,   objCamera.mUp.y,   objCamera.mUp.z);
//    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
//    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
//	glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
//	glTranslatef(0.0f, 0.0f, -10.0f);
//
//    //glCallList(object);
//	
//	glPushMatrix();
//		glTranslatef(-0.5f,-0.5f,0.0f);
//		double xx,yy = 0;
//		double val;
//		int sizzze = linkToMachine->m_grid->m_mesh.size();
//		for (unsigned int i = 0; i <sizzze; ++i)
//		{
//			//for (unsigned int j = 0; j < gridSize; ++j)
//			//{
//
//			//xx = static_cast<double> (i % gridSize);
//			xx = round(linkToMachine->m_grid->m_mesh[i]->getPositionX() / linkToMachine->m_grid->getDeltaR());
//			yy = round(linkToMachine->m_grid->m_mesh[i]->getPositionY() / linkToMachine->m_grid->getDeltaR());// floor(static_cast<double>(i) / static_cast<double>(gridSize));
//				
//				// if(linkToMachine->m_grid->m_mesh[i]->getCellType() == SOLID_WALL ) 
//				// {	//paintCell(i,j,0.01f,0.01f, 0.5 + (linkToMachine->m_oscillators[j][i]->getPotential(linkToMachine->m_whichFlag))/4.0);
//				//	 paintCellPoint(xx, yy, pixelSize, pixelSize, 0, 0, 0, 0, m_palette);
////
//				 //}
//				 if (ELECTROGRAMMM)
//				 {
//					 val = (0.3 + linkToMachine->m_grid->m_mesh[i]->getElectrogram() / 2);
//
//					 paintCellPoint(xx, yy, pixelSize, pixelSize, val, val, val, val, m_palette);
//				 }
//				 else
//				 {//	//paintCell(i,j,0.01f,0.01f, 0.5 + (linkToMachine->m_oscillators[j][i]->getPotential(linkToMachine->m_whichFlag))/4.0);
//					// paintCellPoint(i, j, pixelSize, pixelSize,
//					//(0.3 + (linkToMachine->m_oscillators[j][i]->getPotential())/2.0),
//					//(0.3 + (linkToMachine->m_oscillators[j+1][i]->getPotential())/2.0),
//					//(0.3 + (linkToMachine->m_oscillators[j+1][i+1]->getPotential())/2.0),
//					//(0.3 + (linkToMachine->m_oscillators[j][i+1]->getPotential())/2.0), m_palette );
//				
//				//paintCellPoint(i,j,0.01f,0.01f,
//				//	(linkToMachine->m_diffusionCoefficients[j][i]/linkToMachine->m_definitions->m_maxDiff)*(0.3 + (linkToMachine->m_oscillators[j][i]->getPotential(linkToMachine->m_whichFlag))/2.0),
//				//	(linkToMachine->m_diffusionCoefficients[j+1][i]/linkToMachine->m_definitions->m_maxDiff)*(0.3 + (linkToMachine->m_oscillators[j+1][i]->getPotential(linkToMachine->m_whichFlag))/2.0),
//				//	(linkToMachine->m_diffusionCoefficients[j+1][i+1]/linkToMachine->m_definitions->m_maxDiff)*(0.3 + (linkToMachine->m_oscillators[j+1][i+1]->getPotential(linkToMachine->m_whichFlag))/2.0),
//				//	(linkToMachine->m_diffusionCoefficients[j][i+1]/linkToMachine->m_definitions->m_maxDiff)*(0.3 + (linkToMachine->m_oscillators[j][i+1]->getPotential(linkToMachine->m_whichFlag))/2.0), m_palette );
//				//
//
//				 }
//			
//		}
//		paintProbe(linkToMachine->probeOscillator[0]->getPositionX(),linkToMachine->probeOscillator[0]->getPositionY(),0.01f,0.01f, 1,1,0);
//		paintProbe(linkToMachine->probeOscillator[1]->getPositionX(),linkToMachine->probeOscillator[1]->getPositionY(),0.01f,0.01f, 1,0,0);
//		paintProbe(linkToMachine->probeOscillator[2]->getPositionX(),linkToMachine->probeOscillator[2]->getPositionY(),0.01f,0.01f, 0,1,0);
//
//	glPopMatrix();
}
//----------------------------------------
void glAtrium::resizeGL(int width, int height)
{
	//makeCurrent();
 //   int side = qMin(width, height);
 //   glViewport((width - side) / 2, (height - side) / 2, side, side);
	////gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
 //   glMatrixMode(GL_PROJECTION);
 //   glLoadIdentity();
 //   glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 25.0);
 //   glMatrixMode(GL_MODELVIEW);
	//for (int j = 0; j < linkToMachine->m_grid->getSize(); ++j)
	//{
	//	for (int i = 0; i < linkToMachine->m_grid->getSize(); ++i)
	//	{
	//		m_pixelmap->m_matrix[j][i]->size = min( (static_cast<double>(this->width())) / static_cast<double>(linkToMachine->m_grid->getSize()), (static_cast<double>(this->height())) /static_cast<double>( linkToMachine->m_grid->getSize()));
	//	}

	//}

	makeCurrent();

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	nearClippingPlaneDistance = 2.0;
	glFrustum(-frustrumSize*(static_cast<double>(width) / static_cast<double>(height)),
		+frustrumSize*(static_cast<double>(width) / static_cast<double>(height)),
		+frustrumSize, -frustrumSize, nearClippingPlaneDistance, 50.0);
	fov = 2 * atan((frustrumSize - (-frustrumSize))*0.5 / nearClippingPlaneDistance);
	glMatrixMode(GL_MODELVIEW);


	GLfloat m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	Matrix4 modelMatrix(m);

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
std::pair<int,int> glAtrium::itemAt(const QPoint &pos)
{
	for (int j = linkToMachine->m_grid->getSize() - 1 ; j >= 0; --j) 
	{
		for (int i = linkToMachine->m_grid->getSize() - 1; i >= 0; --i) 
		{
			const Pix *pikselek = m_pixelmap->m_matrix[j][i];
			if (
				QRect(	 pikselek->id_x * pikselek->size,
						 pikselek->id_y * pikselek->size,
						 pikselek->id_x * pikselek->size + pikselek->size,
						 pikselek->id_y * pikselek->size + pikselek->size).contains(pos) )
			{
				std::pair<int,int> temp(i,linkToMachine->m_grid->getSize() - 1-j);
				return temp;
			}
		}
	}
 	std::pair<int,int> temp(-1, -1);
	return temp;
}

void glAtrium::wheelEvent(QWheelEvent *event)
{
	double numDegrees = static_cast<double>(event->delta()) / 350.0;


	distanceToCamera -= numDegrees;
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
	/*if (event->button() == Qt::LeftButton) 
	{
		QPoint position( event->pos().x()-(this->width()-this->height())/2, this->height()-event->pos().y() );
		pair<int,int> temp = itemAt( position );
		int y_index = temp.second;
		int x_index = temp.first;
		if (x_index != -1 && y_index !=-1) 
		{
			if(workMode==3)
			{
				linkToMachine->m_definitions->m_ectopicY = y_index;
				linkToMachine->m_definitions->m_ectopicX = x_index;
				linkToMachine->m_definitions->m_ectopicActivity = true;
			}
		}
		updateGL();
	}*/
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

		//	QPoint position( event->pos().x()-(this->width()-this->height())/2, this->height()-event->pos().y() );
		//	pair<int,int> temp = itemAt( position );
		//	int y_index = temp.second;
		//	int x_index = temp.first;
		//	if (x_index != -1 && y_index !=-1) 
		//	{
		//		if(workMode==3)
		//		{
		//			linkToMachine->m_definitions->m_ectopicY = y_index;
		//			linkToMachine->m_definitions->m_ectopicX = x_index;
		//			linkToMachine->m_definitions->m_ectopicActivity = true;
		//		}
		//	}
		Vector3 pos = screenToWorld(x, y, width, height);
		direction = pos;
		int item = itemAt(pos.x, pos.y, pos.z);
		if (item != -1)
		{
			testProbe.x = linkToMesh->m_mesh[itemAt(pos.x, pos.y, pos.z)]->m_x;
			testProbe.y = linkToMesh->m_mesh[itemAt(pos.x, pos.y, pos.z)]->m_y;
			testProbe.z = linkToMesh->m_mesh[itemAt(pos.x, pos.y, pos.z)]->m_z;
			linkToMachine->m_stimulationID = itemAt(pos.x, pos.y, pos.z);
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
	////if(m_controlable)
	////{
	////	lastPos = event->pos();
	////}
	//if (event->button() == Qt::LeftButton) 
	//{
	//	QPoint position( event->pos().x()-(this->width()-this->height())/2, this->height()-event->pos().y() );
	//	pair<int,int> temp = itemAt( position );
	//	int y_index = temp.second;
	//	int x_index = temp.first;
	//	if (x_index != -1 && y_index !=-1) 
	//	{
	//		if(workMode==3)
	//		{
	//			linkToMachine->m_definitions->m_ectopicY = y_index;
	//			linkToMachine->m_definitions->m_ectopicX = x_index;
	//			linkToMachine->m_definitions->m_ectopicActivity = false;
	//		}
	//	}
	//	updateGL();
	//}

	if (m_controlable)
	{
		lastPos = event->pos();
	}
	if (event->buttons() & Qt::RightButton)
	{



	}

	linkToMachine->m_definitions->m_ectopicActivity = false;
	GLfloat m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	Matrix4 modelMatrix(m);
	//modelMatrix.invert();
	//modelMatrix.transpose();

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
		double height = static_cast<double>(this->height());
		double width = static_cast<double>(this->width());

		Vector3 next = screenToWorld(event->x(), event->y(), width, height);
		Vector3 previous = screenToWorld(lastPos.x(), lastPos.y(), width, height);

		setYRotation(yRot + 1000 * (next.x - previous.x));
		setZRotation(zRot + 1000 * (next.y - previous.y));
		setXRotation(xRot + 1000 * (next.z - previous.z));


		lastPos = event->pos();


	}
	//if(m_controlable)
	//{
	//	int dx = event->x() - lastPos.x();
	//	int dy = event->y() - lastPos.y();

	//	if (event->buttons() & Qt::LeftButton) 
	//	{
	//		setXRotation(xRot - 8 * dy);
	//		setZRotation(zRot + 8 * dx);
	//	} 
	//	else if (event->buttons() & Qt::RightButton) 
	//	{
	//		objCamera.Position_Camera(0.0f,0.0f, objCamera.mPos.z + dy/10.0f, 0.0f, 0.0f, 0.0f,   0, 1, 0);
	//	}
	//	lastPos = event->pos();
	//	updateGL();
	//}
}
//------------------------------------------
GLuint glAtrium::makeObject()
{
    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);

  
	glPushMatrix();
		glTranslatef(-0.5f,-0.5f,0.01f);
	//	paintOrigin();
	glPopMatrix();

    glEndList();
    return list;
}

void glAtrium::quad(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2,
                    GLdouble x3, GLdouble y3, GLdouble x4, GLdouble y4)
{
    qglColor(qtGreen);

    glVertex3d(x1, y1, -0.05);
    glVertex3d(x2, y2, -0.05);
    glVertex3d(x3, y3, -0.05);
    glVertex3d(x4, y4, -0.05);

    glVertex3d(x4, y4, +0.05);
    glVertex3d(x3, y3, +0.05);
    glVertex3d(x2, y2, +0.05);
    glVertex3d(x1, y1, +0.05);
}

void glAtrium::extrude(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
    qglColor(qtGreen.dark(250 + int(100 * x1)));

    glVertex3d(x1, y1, +0.05);
    glVertex3d(x2, y2, +0.05);
    glVertex3d(x2, y2, -0.05);
    glVertex3d(x1, y1, -0.05);
}

void glAtrium::normalizeAngle(int *angle)
{
    while (*angle < 0)
        *angle += 360 * 16;
    while (*angle > 360 * 16)
        *angle -= 360 * 16;
}

Vector3 glAtrium::screenToWorld(double x, double y, double width, double height)
{
	Vector3 pos;

	Vector3 view;
	view.x = 0.0;
	view.y = 0.0;
	view.z = -1.0f;

	Vector3 v;
	v.x = 0.0f;
	v.y = 1.0;
	v.z = 0.0;

	Vector3 h;
	h.x = 1.0f;
	h.y = 0.0f;
	h.z = 0.0;

	double vLength = frustrumSize;
	double hLength = vLength*width / height;

	//v.x *= vLength;
	v.y = v.y * vLength;
	//v.z *= vLength;
	h.x = h.x*hLength;
	//h.y *= hLength;
	//h.z *= hLength;

	// translate mouse coordinates so that the origin lies in the center of the view port
	y -= (height / 2);
	x -= (width / 2);

	// scale mouse coordinates so that half the view port width and height becomes 1
	y = y / ((height / 2));
	x = x / (width / 2);

	// linear combination to compute intersection of picking ray with view port plane
	pos.x = 0 + view.x*nearClippingPlaneDistance + h.x*x + v.x*y; //h.x*x 
	pos.y = 0 + view.y*nearClippingPlaneDistance + h.y*x + v.y*y; //v.y*y
	pos.z = 0 + view.z*nearClippingPlaneDistance + h.z*x + v.z*y; //

	// compute direction of picking ray by subtracting intersection point
	// with camera position
	return pos;

}
void glAtrium::setCurrentPainter_Diffusion()
{
	workMode = 7;
	//update();
	updateGL();
}
void glAtrium::setCurrentPainter_Normal()
{
	workMode = 8;
	//update();
	updateGL();
}
//-----------------------------------------
void glAtrium::setCurrentPainter_Selector()
{
	workMode = 2;
	//update();
	updateGL();
}