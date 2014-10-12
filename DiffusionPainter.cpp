#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "DiffusionPainter.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

//! [0]


//-----------------------------------------------
DiffusionPainter::DiffusionPainter(CardiacMesh* grid, DiffusionMatrix *pixelmap, DiffusionMatrix *anisotrophy, QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{

	initPainter(grid, pixelmap);
	m_anisotrophy = anisotrophy;

	randMachine = new RandomGenerator();
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	
	//objCamera.Position_Camera(0.0f,0.0f, 5.0f, 0.0f, 0.0f, 0.0f,   0, 1, 0);
	defaultColor = parent->palette().color(QPalette::Base);//QColor(255, 200, 255 );
	m_sigma = 3.0;
	m_amplitude = 0.5;
	m_upperLimit = 1.0;
	m_lowerLimit = 0.0;
	m_move_diminisher = 6.0;

	workMode = 1;
	currentMoover = 0;	

    xRot = 0;
    yRot = 0;
    zRot = 0;

	probe1 = new Probe(this);
	probe1->setColor(QColor(255,255,0));
	probe1->setLabel("PROBE1");

	probe2 = new Probe(this);
	probe2->setColor(QColor(255,0,0));
	probe2->setLabel("PROBE2");

	probe3 = new Probe(this);
	probe3->setColor(QColor(0,255,0));
	probe3->setLabel("PROBE3");


	initProbes(5,5,20,20,95,95);
	





	for(int i = 0; i<m_grid->getSize(); ++i) 
	{
		for (int j = 0; j<m_grid->getSize() ; ++j)	
		{
				QColor colorek = m_pixelmap->m_matrix[i][j]->color;
				double xxx = colorek.greenF();
				colorek.setRgbF(xxx, xxx, xxx);
				//if(m_grid->m_net[i][j]->m_type == ATRIAL_TISSUE ) colorek.setRgbF(xxx,xxx,xxx);
				//if(m_grid->m_net[i][j]->m_type == ATRIAL_V3 ) colorek.setRgbF(xxx,xxx,xxx);
				//else if(m_grid->m_net[i][j]->m_type == SA_NODE ) colorek.setRgbF(xxx,xxx,(xxx*0.7));
				//else if(m_grid->m_net[i][j]->m_type == AV_NODE ) colorek.setRgbF((xxx*0.7),xxx,(xxx*0.7));
				//else if(m_grid->m_net[i][j]->m_type == AV_NODE_BRANCH ) colorek.setRgbF((xxx*0.8),xxx,(xxx*0.5));
				//else if(m_grid->m_net[i][j]->m_type == ATRIAL_BRANCH ) colorek.setRgbF((xxx*0.7),xxx,xxx);
				//else if(m_grid->m_net[i][j]->m_type == PURKINIE_BUNDLE ) colorek.setRgbF((xxx*0.7),(xxx*0.7),xxx);
				//else if(m_grid->m_net[i][j]->m_type == SOLID_WALL ) colorek.setRgbF(0,0,0);
				//else colorek.setRgbF(xxx,xxx,xxx);
				m_pixelmap->m_matrix[i][j]->color = colorek;
		}
	}

	for(int i = 0; i<m_grid->getSize(); ++i) 
	{
		for (int j = 0; j<m_grid->getSize() ; ++j)	
		{
				QColor colorek = m_pixelmap->m_matrix[i][j]->color;
				//colorek.setRgbF(m_grid->m_net[i][j]->m_anisotrophy_L, m_grid->m_net[i][j]->m_anisotrophy_R, 0.0);
				double xxx = colorek.greenF();
				colorek.setRgbF(xxx, xxx, xxx);
				m_anisotrophy->m_matrix[i][j]->color = colorek;
		}
	}
}
//------------------------------------------------------
DiffusionPainter::~DiffusionPainter()
{

}
//------------------------------------------------------
void DiffusionPainter::initPainter( CardiacMesh* grid, DiffusionMatrix *pixelmap)
{
	//destroyPainter();
	m_pixelmap = pixelmap;
	m_grid = grid;
}

//--------------------------------------------
void DiffusionPainter::initProbes(int e1_x,int e1_y, 
						  int e2_x,int e2_y,
						  int e3_x,int e3_y)
{
	probe1->setPosition(QPoint(  e1_x ,e1_y ));
	probe2->setPosition(QPoint(  e2_x ,e2_y ));
	probe3->setPosition(QPoint(  e3_x ,e3_y ));
	probe1on = true;
	probe2on = false;
	probe3on = false;
	currentMoover = 0;
	currentlySelected = m_pixelmap->m_matrix[e1_x][e1_y];
}
//-------------------------------------------------------
void DiffusionPainter::destroyPainter()
{
	m_pixelmap->clear();
	//count = 1;
}
//-------------------------------------------------------------------
QSize DiffusionPainter::minimumSizeHint() const
{
    return QSize(50, 50);
}
//! [2]

//! [3]
QSize DiffusionPainter::sizeHint() const
//! [3] //! [4]
{
    return QSize(255, 255);
}
//! [4]

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

//! [5]
void DiffusionPainter::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}
//! [5]

void DiffusionPainter::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void DiffusionPainter::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

//! [6]
void DiffusionPainter::initializeGL()
{
    qglClearColor(qtPurple.dark());
	
	glShadeModel(GL_SMOOTH);						// Enables Smooth Shading
	glClearDepth(1.0f);							// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);							// The Type Of Depth Test To Do
//	gluPerspective(45.0f,(GLfloat)(this->width())/(GLfloat)(this->height()),0.01f,1000.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);



	//for (int j = 0; j < m_grid->getSizeY(); ++j)
	//{
	//	for (int i = 0; i < m_grid->getSizeX(); ++i)
	//	{
	//		m_pixelmap->m_matrix[j][i]->size = min( (static_cast<double>(this->width())) / static_cast<double>( m_grid->getSizeX()), (static_cast<double>(this->height())) /static_cast<double>( m_grid->getSizeY()));
	//		m_pixelSize= min( (static_cast<double>(this->width())) / static_cast<double>( m_grid->getSizeX()), (static_cast<double>(this->height())) /static_cast<double>( m_grid->getSizeY()));
	//		m_anisotrophy->m_matrix[j][i]->size = min( (static_cast<double>(this->width())) / static_cast<double>( m_grid->getSizeX()), (static_cast<double>(this->height())) /static_cast<double>( m_grid->getSizeY()));
	//		
	//	}
//
	//}
}
//! [6]

//! [7]
void DiffusionPainter::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

//	gluLookAt(objCamera.mPos.x,  objCamera.mPos.y,  objCamera.mPos.z,	
//		  objCamera.mView.x, objCamera.mView.y, objCamera.mView.z,	
//			  objCamera.mUp.x,   objCamera.mUp.y,   objCamera.mUp.z);
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
	
	glTranslatef(0.0f,0.0f,-5.0f);
    qglClearColor( defaultColor );
	glPushMatrix();
		glTranslatef(-0.5f,-0.51f,0.0f);

		if(workMode != 6)
		{
			for(int i =0 ; i < m_grid->getSize(); ++i)
			{
				for(int j =0 ; j < m_grid->getSize(); ++j)
				{
					paintPixel(j, m_grid->getSize()-i,0.01f,0.01f, m_pixelmap->m_matrix[i][j]->color );
				}
			}
		}
		else
		{
			for(int i =0 ; i < m_grid->getSize(); ++i)
			{
				for(int j =0 ; j < m_grid->getSize(); ++j)
				{
					paintPixel(j, m_grid->getSize()-i,0.01f,0.01f, m_anisotrophy->m_matrix[i][j]->color );
				}
			}
		}
	glPopMatrix();

	glTranslatef(-0.5f,0.5f,0.0f);
	glTranslatef(static_cast<double>(probe1->position().y())/static_cast<double>(m_grid->getSize()),
				-static_cast<double>(probe1->position().x())/static_cast<double>(m_grid->getSize()),0.0f);
	 probe1->draw();
	glTranslatef(-static_cast<double>(probe1->position().y())/static_cast<double>(m_grid->getSize()),
				static_cast<double>(probe1->position().x())/static_cast<double>(m_grid->getSize()),0.0f);

	glTranslatef(static_cast<double>(probe2->position().y())/static_cast<double>(m_grid->getSize()),
				-static_cast<double>(probe2->position().x())/static_cast<double>(m_grid->getSize()),0.0f);
	 probe2->draw();
	glTranslatef(-static_cast<double>(probe2->position().y())/static_cast<double>(m_grid->getSize()),
				static_cast<double>(probe2->position().x())/static_cast<double>(m_grid->getSize()),0.0f);

	glTranslatef(static_cast<double>(probe3->position().y())/static_cast<double>(m_grid->getSize()),
				-static_cast<double>(probe3->position().x())/static_cast<double>(m_grid->getSize()),0.0f);
	 probe3->draw();
	glTranslatef(-static_cast<double>(probe3->position().y())/static_cast<double>(m_grid->getSize()),
				static_cast<double>(probe3->position().x())/static_cast<double>(m_grid->getSize()),0.0f);
}
//! [7]

//! [8]
void DiffusionPainter::resizeGL(int width, int height)
{

		makeCurrent();


    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#else
    glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);

	for (int j = 0; j < m_grid->getSize(); ++j)
	{

		for (int i = 0; i < m_grid->getSize(); ++i)
		{
			m_pixelmap->m_matrix[j][i]->size = min( (static_cast<double>(this->width())) / static_cast<double>( m_grid->getSize()), (static_cast<double>(this->height())) /( static_cast<double>(m_grid->getSize())));
			m_pixelSize= min( (static_cast<double>(this->width())) / static_cast<double>( m_grid->getSize()), (static_cast<double>(this->height())) /static_cast<double>( m_grid->getSize()));
			m_anisotrophy->m_matrix[j][i]->size = min( (static_cast<double>(this->width())) / static_cast<double>( m_grid->getSize()), (static_cast<double>(this->height())) /static_cast<double>( m_grid->getSize()));
			
		}

	}
}
//! [8]

//! [9]
void DiffusionPainter::mousePressEvent(QMouseEvent *event)
{
    //lastPos = event->pos();
	if (event->button() == Qt::RightButton) 
	{
	}
	if (event->button() == Qt::LeftButton) 
	{
		QPoint position( event->pos().x()-(this->width()-this->height())/2, this->height()-event->pos().y() );
		pair<int,int> temp = this->itemAt( position );
		emitWidth( temp.first);
		int y_index = temp.second;
		int x_index = temp.first;
		if (x_index != -1 && y_index !=-1) 
		{
			if(workMode==2)
			{
					if( (currentMoover == 0) && ( probe1->isHidden ) ) {}
					else if( (currentMoover == 1)&& ( probe2->isHidden ) ) {}
					else if( (currentMoover == 2) && ( probe3->isHidden ) ) {}
					else
					{
						emit positionElektrode( currentMoover,	y_index, x_index);
						currentlyPressed[currentMoover] = m_pixelmap->m_matrix[y_index][x_index];
						if(currentMoover == 0) probe1->setPosition(QPoint( y_index ,  x_index ));
						if(currentMoover == 1) probe2->setPosition(QPoint( y_index ,  x_index ));
						if(currentMoover == 2) probe3->setPosition(QPoint( y_index ,  x_index ));
					}
			}
			else if(workMode==6)
			{
				x_index = temp.first;
				y_index =  temp.second;
				for(int i = 0; i<m_grid->getSize(); ++i) 
				{
					for (int j = 0; j<m_grid->getSize() ; ++j)	
					{
							QColor colorek = m_anisotrophy->m_matrix[i][j]->color;
							double xxxLEFT = colorek.redF();
							double radius = (j-x_index)*(j-x_index) + (i-y_index)*(i-y_index);
							radius = sqrt(radius);
							if(xxxLEFT <m_amplitude)
							{
								xxxLEFT += randMachine->gaussFunction(radius,0,m_sigma)/m_move_diminisher;
								if(xxxLEFT >= m_amplitude) xxxLEFT = m_amplitude;
							}
							if(xxxLEFT >m_amplitude)
							{
								xxxLEFT -= randMachine->gaussFunction(radius,0,m_sigma)/m_move_diminisher;
								if(xxxLEFT <= m_amplitude) xxxLEFT = m_amplitude;
							}
							xxxLEFT = xxxLEFT * 1000;
							int z = static_cast<int> ( xxxLEFT);
							xxxLEFT = ((double)z)/1000.0;

							//m_grid->m_net[i][j]->m_anisotrophy_L = xxxLEFT;
							//m_grid->m_net[i][j]->m_anisotrophy_R = 1.0 - xxxLEFT;

							//colorek.setRgbF(m_grid->m_net[i][j]->m_anisotrophy_L,m_grid->m_net[i][j]->m_anisotrophy_R,0.0);
							m_anisotrophy->m_matrix[i][j]->color = colorek;
					}
				}
			}
			else if(workMode==1)
			{
				x_index = temp.first;
				 y_index =  temp.second;
				for(int i = 0; i<m_grid->getSize(); ++i) 
				{
					for (int j = 0; j<m_grid->getSize() ; ++j)	
					{
							QColor colorek = m_pixelmap->m_matrix[i][j]->color;
							double xxx = colorek.greenF();
							double radius = (j-x_index)*(j-x_index) + (i-y_index)*(i-y_index);
							radius = sqrt(radius);
							if(xxx <m_amplitude)
							{
								xxx += randMachine->gaussFunction(radius,0,m_sigma)/m_move_diminisher;
								if(xxx >= m_amplitude) xxx = m_amplitude;
							}
							if(xxx >m_amplitude)
							{
								xxx -= randMachine->gaussFunction(radius,0,m_sigma)/m_move_diminisher;
								if(xxx <= m_amplitude) xxx = m_amplitude;
							}
							xxx = xxx * 1000;
							int z = static_cast<int> ( xxx);
							xxx = ((double)z)/1000.0;
							colorek.setRgbF(xxx, xxx, xxx);
				/*			if(m_grid->m_net[i][j]->m_type == ATRIAL_TISSUE ) colorek.setRgbF(xxx,xxx,xxx);
							if(m_grid->m_net[i][j]->m_type == ATRIAL_V3 ) colorek.setRgbF(xxx,xxx,xxx);
							else if(m_grid->m_net[i][j]->m_type == SA_NODE ) colorek.setRgbF(xxx,xxx,(xxx*0.7));
							else if(m_grid->m_net[i][j]->m_type == AV_NODE ) colorek.setRgbF((xxx*0.7),xxx,(xxx*0.7));
							else if(m_grid->m_net[i][j]->m_type == AV_NODE_BRANCH ) colorek.setRgbF((xxx*0.8),xxx,(xxx*0.5));
							else if(m_grid->m_net[i][j]->m_type == ATRIAL_BRANCH ) colorek.setRgbF((xxx*0.7),xxx,xxx);
							else if(m_grid->m_net[i][j]->m_type == PURKINIE_BUNDLE ) colorek.setRgbF((xxx*0.7),(xxx*0.7),xxx);
							else if(m_grid->m_net[i][j]->m_type == SOLID_WALL ) colorek.setRgbF(0,0,0);
							else colorek.setRgbF(xxx,xxx,xxx);*/
							//colorek.setRgbF(xxx,xxx,xxx);
							m_pixelmap->m_matrix[i][j]->color = colorek;
					}
				}

			}
		}
		updateGL();
	}

}
//! [9]
std::pair<int,int> DiffusionPainter::itemAt(const QPoint &pos)
{
	double minimizer = min( this->width(), (this->height()));
	for (int j = m_grid->getSize() - 1 ; j >= 0; --j) 
	{
		for (int i = m_grid->getSize() - 1; i >= 0; --i) 
		{
			const Pix *pikselek = m_pixelmap->m_matrix[j][i];
			if (
				QRect(	 pikselek->id_x * m_pixelSize,
						 pikselek->id_y * m_pixelSize,
						 pikselek->id_x * m_pixelSize + m_pixelSize,
						 pikselek->id_y * m_pixelSize + m_pixelSize).contains(pos) )
			{
				std::pair<int,int> temp(i,m_grid->getSize() - 1-j);
				return temp;
			}
		}
	}
 	std::pair<int,int> temp(-1, -1);
	return temp;
}

//! [10]

void DiffusionPainter::updateCanvas()
{
		for(int i = 0; i<m_grid->getSize(); ++i) 
				{
					for (int j = 0; j<m_grid->getSize() ; ++j)	
					{
							QColor colorek = m_pixelmap->m_matrix[i][j]->color;
							double xxx = colorek.greenF();
							//if(m_grid->m_net[i][j]->m_type == ATRIAL_TISSUE ) colorek.setRgbF(xxx,xxx,xxx);
							//if(m_grid->m_net[i][j]->m_type == ATRIAL_V3 ) colorek.setRgbF(xxx,xxx,xxx);
							//else if(m_grid->m_net[i][j]->m_type == SA_NODE ) colorek.setRgbF(xxx,xxx,(xxx*0.7));
							//else if(m_grid->m_net[i][j]->m_type == AV_NODE ) colorek.setRgbF((xxx*0.7),xxx,(xxx*0.7));
							//else if(m_grid->m_net[i][j]->m_type == AV_NODE_BRANCH ) colorek.setRgbF((xxx*0.8),xxx,(xxx*0.5));
							//else if(m_grid->m_net[i][j]->m_type == ATRIAL_BRANCH ) colorek.setRgbF((xxx*0.7),xxx,xxx);
							//else if(m_grid->m_net[i][j]->m_type == PURKINIE_BUNDLE ) colorek.setRgbF((xxx*0.7),(xxx*0.7),xxx);
							//else if(m_grid->m_net[i][j]->m_type == SOLID_WALL ) colorek.setRgbF(0,0,0);
							//else colorek.setRgbF(xxx,xxx,xxx);
							colorek.setRgbF(xxx,xxx,xxx);
							m_pixelmap->m_matrix[i][j]->color = colorek;
					}
				}

}
void DiffusionPainter::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::RightButton) {
        //setXRotation(xRot + 8 * dy);
       // setYRotation(yRot + 8 * dx);
    } 
	//else if (event->buttons() & Qt::RightButton) {
    //    setXRotation(xRot + 8 * dy);
    //    setZRotation(zRot + 8 * dx);
    //}
    //lastPos = event->pos();

	if ((event->buttons() & Qt::LeftButton )&& workMode==1)
	{

		QPoint position( event->pos().x()-(this->width()-this->height())/2, this->height()-event->pos().y() );
		pair<int,int> temp = itemAt( position );
		int x_index = temp.first;
		int y_index = temp.second;
		//int x_index = (event->pos().x())/ (0.5+min( this->width() /  m_grid->getSizeX(), this->height() / m_grid->getSizeY())); //temp.second;
		//int y_index =(event->pos().y())/ (0.5+min( this->width() /  m_grid->getSizeX(), this->height() / m_grid->getSizeY()));// temp.first;
		if (x_index != -1 && y_index !=-1) 
		{
			//if(currentCellPainter != NONE)
			//{
				for(int i = 0; i<m_grid->getSize(); ++i) 
				{
					for (int j = 0; j<m_grid->getSize() ; ++j)	
					{
							QColor colorek = m_pixelmap->m_matrix[i][j]->color;
							double xxx = colorek.greenF();
							//if(m_grid->m_net[i][j]->m_type == PURKINIE_BUNDLE ) xxx = colorek.blueF();
							double radius = (j-x_index)*(j-x_index) + (i-y_index)*(i-y_index);
							radius = sqrt(radius);
							if(xxx <m_amplitude)
							{
								xxx += randMachine->gaussFunction(radius,0,m_sigma)/m_move_diminisher;
								if(xxx >= m_amplitude) xxx = m_amplitude;
							}
							if(xxx >m_amplitude)
							{
								xxx -= randMachine->gaussFunction(radius,0,m_sigma)/m_move_diminisher;
								if(xxx <= m_amplitude) xxx = m_amplitude;
							}
							xxx = xxx * 1000;
							int z = static_cast<int> ( xxx);
							xxx = ((double)z)/1000.0;
							//if(m_grid->m_net[i][j]->m_type == ATRIAL_TISSUE ) colorek.setRgbF(xxx,xxx,xxx);
							//if(m_grid->m_net[i][j]->m_type == ATRIAL_V3 ) colorek.setRgbF(xxx,xxx,xxx);
							//else if(m_grid->m_net[i][j]->m_type == SA_NODE ) colorek.setRgbF(xxx,xxx,(xxx*0.7));
							//else if(m_grid->m_net[i][j]->m_type == AV_NODE ) colorek.setRgbF((xxx*0.7),xxx,(xxx*0.7));
							//else if(m_grid->m_net[i][j]->m_type == AV_NODE_BRANCH ) colorek.setRgbF((xxx*0.8),xxx,(xxx*0.5));
							//else if(m_grid->m_net[i][j]->m_type == ATRIAL_BRANCH ) colorek.setRgbF((xxx*0.7),xxx,xxx);
							//else if(m_grid->m_net[i][j]->m_type == PURKINIE_BUNDLE ) colorek.setRgbF((xxx*0.7),(xxx*0.7),xxx);
							//else if(m_grid->m_net[i][j]->m_type == SOLID_WALL ) colorek.setRgbF(0,0,0);
							//else colorek.setRgbF(xxx,xxx,xxx);
							colorek.setRgbF(xxx,xxx,xxx);
							m_pixelmap->m_matrix[i][j]->color = colorek;
						
					}
				
				}
				updateGL();
			//}
			//else 
			//{

	          
			//}
			
		}
	}



	if ((event->buttons() & Qt::LeftButton )&& workMode==6)
	{

		QPoint position( event->pos().x()-(this->width()-this->height())/2, this->height()-event->pos().y() );
		pair<int,int> temp = itemAt( position );
		int x_index = temp.first;
		int y_index = temp.second;
		//int x_index = (event->pos().x())/ (0.5+min( this->width() /  m_grid->getSizeX(), this->height() / m_grid->getSizeY())); //temp.second;
		//int y_index =(event->pos().y())/ (0.5+min( this->width() /  m_grid->getSizeX(), this->height() / m_grid->getSizeY()));// temp.first;
		if (x_index != -1 && y_index !=-1) 
		{
			//if(currentCellPainter != NONE)
			//{
				for(int i = 0; i<m_grid->getSize(); ++i) 
				{
					for (int j = 0; j<m_grid->getSize() ; ++j)	
					{
						
							QColor colorek = m_anisotrophy->m_matrix[i][j]->color;
							double xxxLEFT = colorek.redF();
							double radius = (j-x_index)*(j-x_index) + (i-y_index)*(i-y_index);
							radius = sqrt(radius);
							if(xxxLEFT <m_amplitude)
							{
								xxxLEFT += randMachine->gaussFunction(radius,0,m_sigma)/m_move_diminisher;
								if(xxxLEFT >= m_amplitude) xxxLEFT = m_amplitude;
							}
							if(xxxLEFT >m_amplitude)
							{
								xxxLEFT -= randMachine->gaussFunction(radius,0,m_sigma)/m_move_diminisher;
								if(xxxLEFT <= m_amplitude) xxxLEFT = m_amplitude;
							}
							xxxLEFT = xxxLEFT * 1000;
							int z = static_cast<int> ( xxxLEFT);
							xxxLEFT = ((double)z)/1000.0;
/*
							m_grid->m_net[i][j]->m_anisotrophy_L = xxxLEFT;
							m_grid->m_net[i][j]->m_anisotrophy_R = 1.0 - xxxLEFT;

							colorek.setRgbF(m_grid->m_net[i][j]->m_anisotrophy_L,m_grid->m_net[i][j]->m_anisotrophy_R,0.0);*/
							m_anisotrophy->m_matrix[i][j]->color = colorek;
													
					}
				
				}
				updateGL();
			//}
			//else 
			//{

	          
			//}
			
		}
	}
}


//-----------------------------------------
void DiffusionPainter::setCurrentPainter_Diffusion()
{
	workMode = 1;
	//update();
	updateGL();
}
//-----------------------------------------
void DiffusionPainter::setCurrentPainter_Selector()
{
	workMode = 2;
	//update();
	updateGL();
}
//-----------------------------------------
void DiffusionPainter::setCurrentPainter_Bezier1()
{
	workMode = 3;
	//update();
	updateGL();
}
//-----------------------------------------
void DiffusionPainter::setCurrentPainter_Anisotrophy()
{
	workMode = 6;
	//update();
	updateGL();
}
//-----------------------------------------
void DiffusionPainter::setCurrentPainter_Bezier2()
{
	workMode = 4;
	//update();
	updateGL();
}
//-----------------------------------------
void DiffusionPainter::setSigma (int value)
{
	m_sigma = (static_cast<double>(value));
}
//-----------------------------------------
void DiffusionPainter::setUpperLimit(double value)
{
	m_upperLimit = value;
}
void DiffusionPainter::setLowerLimit(double value)
{	
	m_lowerLimit = value;
}
void DiffusionPainter::setAmplitude(int value)
{
	m_amplitude = (static_cast<double>(value)/10) ;
}