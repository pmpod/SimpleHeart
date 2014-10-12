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
glAtrium::glAtrium( const AtrialMachine2d *link, DiffusionMatrix *pixelmap, QWidget *parent)
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

	objCamera.Position_Camera(0.0f,0.0f, 10.0f, 0.0f, 0.0f, 0.0f,   0, 1, 0);
    linkToMachine = link;
    m_pixelmap=pixelmap;

	m_controlable = true;
	workMode=3;
	
	pixelSize = 1.0f / static_cast<double>( link->m_grid->getSize());
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
    return QSize(500, 500);
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
//	gluPerspective(45.0f,(GLfloat)(sizeHint().width())/(GLfloat)(sizeHint().height()),0.01f,1000.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//gridSize = linkToMachine->m_grid->m_net.size() - 1;
	gridSize = linkToMachine->m_grid->getSize();
   // glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
	for (int j = 0; j < linkToMachine->m_grid->getSize(); ++j)
	{
		for (int i = 0; i < linkToMachine->m_grid->getSize(); ++i)
		{
			double a = (static_cast<double>(this->width())) / static_cast<double>(linkToMachine->m_grid->getSize());
			double b = (static_cast<double>(this->height())) / static_cast<double>(linkToMachine->m_grid->getSize());
			m_pixelmap->m_matrix[j][i]->size = min( (static_cast<double>(this->width())) / static_cast<double>(linkToMachine->m_grid->getSize()), (static_cast<double>(this->height())) /static_cast<double>( linkToMachine->m_grid->getSize()));
		}

	}
}
//----------------------------------------
void glAtrium::paintGL()
{
		//makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    qglClearColor( colorek );
    glLoadIdentity();

	//gluLookAt(objCamera.mPos.x,  objCamera.mPos.y,  objCamera.mPos.z,	
	//	  objCamera.mView.x, objCamera.mView.y, objCamera.mView.z,	
	//		  objCamera.mUp.x,   objCamera.mUp.y,   objCamera.mUp.z);
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
	glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
	glTranslatef(0.0f, 0.0f, -10.0f);

    //glCallList(object);
	
	glPushMatrix();
		glTranslatef(-0.5f,-0.5f,0.0f);
		double xx,yy = 0;
		double val;
		int sizzze = linkToMachine->m_grid->m_mesh.size();
		for (unsigned int i = 0; i <sizzze; ++i)
		{
			//for (unsigned int j = 0; j < gridSize; ++j)
			//{

			//xx = static_cast<double> (i % gridSize);
			xx = round(linkToMachine->m_grid->m_mesh[i]->getPositionX() / linkToMachine->m_grid->getDeltaR());
			yy = round(linkToMachine->m_grid->m_mesh[i]->getPositionY() / linkToMachine->m_grid->getDeltaR());// floor(static_cast<double>(i) / static_cast<double>(gridSize));
				
				// if(linkToMachine->m_grid->m_mesh[i]->getCellType() == SOLID_WALL ) 
				// {	//paintCell(i,j,0.01f,0.01f, 0.5 + (linkToMachine->m_oscillators[j][i]->getPotential(linkToMachine->m_whichFlag))/4.0);
				//	 paintCellPoint(xx, yy, pixelSize, pixelSize, 0, 0, 0, 0, m_palette);
//
				 //}
				 if (ELECTROGRAMMM)
				 {
					 val = (0.3 + linkToMachine->m_grid->m_mesh[i]->getElectrogram() / 2);

					 paintCellPoint(xx, yy, pixelSize, pixelSize, val, val, val, val, m_palette);
				 }
				 else
				 {//	//paintCell(i,j,0.01f,0.01f, 0.5 + (linkToMachine->m_oscillators[j][i]->getPotential(linkToMachine->m_whichFlag))/4.0);
					// paintCellPoint(i, j, pixelSize, pixelSize,
					//(0.3 + (linkToMachine->m_oscillators[j][i]->getPotential())/2.0),
					//(0.3 + (linkToMachine->m_oscillators[j+1][i]->getPotential())/2.0),
					//(0.3 + (linkToMachine->m_oscillators[j+1][i+1]->getPotential())/2.0),
					//(0.3 + (linkToMachine->m_oscillators[j][i+1]->getPotential())/2.0), m_palette );
				
				//paintCellPoint(i,j,0.01f,0.01f,
				//	(linkToMachine->m_diffusionCoefficients[j][i]/linkToMachine->m_definitions->m_maxDiff)*(0.3 + (linkToMachine->m_oscillators[j][i]->getPotential(linkToMachine->m_whichFlag))/2.0),
				//	(linkToMachine->m_diffusionCoefficients[j+1][i]/linkToMachine->m_definitions->m_maxDiff)*(0.3 + (linkToMachine->m_oscillators[j+1][i]->getPotential(linkToMachine->m_whichFlag))/2.0),
				//	(linkToMachine->m_diffusionCoefficients[j+1][i+1]/linkToMachine->m_definitions->m_maxDiff)*(0.3 + (linkToMachine->m_oscillators[j+1][i+1]->getPotential(linkToMachine->m_whichFlag))/2.0),
				//	(linkToMachine->m_diffusionCoefficients[j][i+1]/linkToMachine->m_definitions->m_maxDiff)*(0.3 + (linkToMachine->m_oscillators[j][i+1]->getPotential(linkToMachine->m_whichFlag))/2.0), m_palette );
				//

				 }
			
		}
		paintProbe(linkToMachine->probeOscillator[0]->getPositionX(),linkToMachine->probeOscillator[0]->getPositionY(),0.01f,0.01f, 1,1,0);
		paintProbe(linkToMachine->probeOscillator[1]->getPositionX(),linkToMachine->probeOscillator[1]->getPositionY(),0.01f,0.01f, 1,0,0);
		paintProbe(linkToMachine->probeOscillator[2]->getPositionX(),linkToMachine->probeOscillator[2]->getPositionY(),0.01f,0.01f, 0,1,0);

	glPopMatrix();
}
//----------------------------------------
void glAtrium::resizeGL(int width, int height)
{
	makeCurrent();
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
	//gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 25.0);
    glMatrixMode(GL_MODELVIEW);
	for (int j = 0; j < linkToMachine->m_grid->getSize(); ++j)
	{
		for (int i = 0; i < linkToMachine->m_grid->getSize(); ++i)
		{
			m_pixelmap->m_matrix[j][i]->size = min( (static_cast<double>(this->width())) / static_cast<double>(linkToMachine->m_grid->getSize()), (static_cast<double>(this->height())) /static_cast<double>( linkToMachine->m_grid->getSize()));
		}

	}
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

void glAtrium::mousePressEvent(QMouseEvent *event)
{
	//if(m_controlable)
	//{
	//	lastPos = event->pos();
	//}
	if (event->button() == Qt::LeftButton) 
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
	}



}

void glAtrium::mouseReleaseEvent(QMouseEvent *event)
{
	//if(m_controlable)
	//{
	//	lastPos = event->pos();
	//}
	if (event->button() == Qt::LeftButton) 
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
				linkToMachine->m_definitions->m_ectopicActivity = false;
			}
		}
		updateGL();
	}



}
//----------------------------------------
void glAtrium::mouseMoveEvent(QMouseEvent *event)
{
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
		paintOrigin();
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