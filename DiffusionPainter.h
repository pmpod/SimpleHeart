#ifndef DiffusionPainter_H
#define DiffusionPainter_H

#include <QGLWidget>
#include "Probe.h"
#include "heartDefines.h"
#include "cartesianGrid.h"
#include "RandomGenerator.h"
#include "DiffusionMatrix.h"
#include "CardiacMesh.h"

#include "GLfunc.h"
#include <assert.h>

class DiffusionPainter : public QGLWidget
{
    Q_OBJECT

public:
	DiffusionPainter(CardiacMesh* grid, DiffusionMatrix *pixelmap, DiffusionMatrix *anisotrophy, QWidget *parent = 0);
    ~DiffusionPainter();

	void initPainter(CardiacMesh* grid, DiffusionMatrix *pixelmap);
	void initProbes(int e1_x,int e1_y, int e2_x,int e2_y, int e3_x,int e3_y);
	void destroyPainter();

	CCamera objCamera;
    QSize minimumSizeHint() const;
    QSize sizeHint() const;


	CardiacMesh *m_grid;
	RandomGenerator *randMachine;
    DiffusionMatrix *m_pixelmap;
    DiffusionMatrix *m_anisotrophy;


	Probe *probe1;
	Probe *probe2;
	Probe *probe3;
	bool probe1on;
	bool probe2on;
	bool probe3on;
	int currentMoover;
	Pix *currentlyPressed[3];
	double m_pixelSize;
////
	double m_sigma;
	double m_amplitude;
	double m_move_diminisher;
	double m_upperLimit;
	double m_lowerLimit;
	int workMode;

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
	void setCurrentPainter_Diffusion();
	void setCurrentPainter_Selector();
	void setCurrentPainter_Bezier1();
	void setCurrentPainter_Bezier2();
	void setCurrentPainter_Anisotrophy();
	void setSigma (int value);
	void setUpperLimit(double value);
	void setLowerLimit(double value);
	void setAmplitude(int value);
	void updateCanvas();
signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
	void selectedNode(int,int);
	void positionElektrode(int,int,int);
	void emitWidth(double);
	void foundPoint(int,int);
//! [1]

//! [2]
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
//! [2]

//! [3]
private:

	std::pair<int,int> itemAt(const QPoint &pos);

	Pix *currentlySelected;
    int xRot;
    int yRot;
    int zRot;
    QPoint lastPos;
    QColor qtGreen;
    QColor qtPurple;

	QColor defaultColor;
};
//! [3]

#endif


//#pragma once
//#include <QGLWidget>
//#include <QtGui>
//#include "oscProbe.h"
//#include "heartDefines.h"
//#include "cartesianGrid.h"
//#include "RandomGenerator.h"
//#include "GLcamera.h"
//#include "GLfunc.h"
//
////class Pix
////{
////public:
////	double size;
////	QColor color;
////	int id_x;
////	int id_y;
////	QPoint position;
////};
//
//class DiffusionPainter: public QGLWidget
//{
//	Q_OBJECT
//public:
//	DiffusionPainter();
////	void initPainter(heartDefines *definitions, CartesianGrid* grid);
////	void destroyPainter();
////
//    QSize minimumSizeHint() const;
//    QSize sizeHint() const;
////	CCamera objCamera;
////
//	~DiffusionPainter(void);
////
////	CartesianGrid *m_grid;
////	heartDefines *m_definitions;
////	//RandomGenerator *randMachine;
////    vector<vector<Pix*>> m_pixelmap;
////
////
////	oscProbe *probe1;
////	oscProbe *probe2;
////	oscProbe *probe3;
////	bool probe1on;
////	bool probe2on;
////	bool probe3on;
////	int currentMoover;
////	//OscIcon *currentlyPressed[3];
////
////	double m_sigma;
////	double m_amplitude;
////	double m_move_diminisher;
////	double m_upperLimit;
////	double m_lowerLimit;
////
////signals:
////	void selectedNode(int,int);
////	void positionElektrode(int,int,int);
////public slots:
////	void setCurrentPainter_Diffusion();
////	void setCurrentPainter_Selector();
////	void setCurrentPainter_Bezier1();
////	void setCurrentPainter_Bezier2();
////	void setSigma (double value);
////	void setUpperLimit(double value);
////	void setLowerLimit(double value);
////	void setAmplitude(double value);
////
////protected:
//    void initializeGL();
////    void paintGL();
////    void resizeGL(int width, int height);
////    void mousePressEvent(QMouseEvent *event);
////    void mouseMoveEvent(QMouseEvent *event);
////    void mouseReleaseEvent(QMouseEvent *event);
//
//private:
////std::pair<int,int> itemAt(const QPoint &pos);
////Pix *currentlySelected;
////int m_offset;
////
////GLuint makeObject();
////QColor defaultColor;
////int workMode;
//};
