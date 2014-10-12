#pragma once
#include <QGLWidget>
#include "AtrialMachine2d.h"
#include "GLcamera.h"
#include "GLfunc.h"
#include "DiffusionMatrix.h"


class glAtrium: public QGLWidget
{
	Q_OBJECT

public:
	glAtrium(const AtrialMachine2d *link, DiffusionMatrix *pixelmap, QWidget *parent = 0);
	~glAtrium(void);

	CCamera objCamera;

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

	bool m_controlable;
	int m_palette;

	
    DiffusionMatrix *m_pixelmap;
	int workMode;
//! [0]

//! [1]
public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
	void changeColor(double value);
	void setPaletteHSV();
	void setPaletteGray();
	void setTop();
	void setSide1();
	void setSide2();

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
//! [1]

//! [2]
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


//! [2]

//! [3]
private:

	std::pair<int,int> itemAt(const QPoint &pos);

    GLuint makeObject();
    void quad(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2,
              GLdouble x3, GLdouble y3, GLdouble x4, GLdouble y4);
    void extrude(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
    void normalizeAngle(int *angle);

	//void paintGround();
	//void paintOrigin();
	void paintPotential();
	QColor colorek;
    GLuint object;
    int xRot;
    int yRot;
    int zRot;

	float pixelSize;
	int gridSize;
    QPoint lastPos;
    QColor qtGreen;
    QColor qtPurple;

	const AtrialMachine2d *linkToMachine;
};
