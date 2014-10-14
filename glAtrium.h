#pragma once
#include <QGLWidget>
#include "AtrialMachine2d.h"
#include "GLcamera.h"
#include "GLfunc.h"
#include "DiffusionMatrix.h"
#include "Model\CardiacMesh.h"
#include "Support\Vectors.h"
#include "Support\Matrices.h"

//TODO Update to OpenGL 3.0
class glAtrium: public QGLWidget
{
	Q_OBJECT

public:
	glAtrium(CardiacMesh *linkMesh, AtrialMachine2d *link, DiffusionMatrix *pixelmap, QWidget *parent = 0);
	~glAtrium(void);

	CCamera objCamera;

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
	Vector3 screenToWorld(double x, double y, double width, double height);

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
	void setTop();
	void setSide1();
	void setSide2();

	void changeColor(double value);
	void setPaletteHSV();
	void setPaletteGray();


	void setCurrentPainter_Diffusion();
	void setCurrentPainter_Selector();
	void setCurrentPainter_Normal();

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
	void wheelEvent(QWheelEvent *event);


//! [2]

//! [3]
private:

	std::pair<int, int> itemAt(const QPoint &pos);
	int itemAt(double xx, double yy, double zz);

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

	AtrialMachine2d *linkToMachine;
	const CardiacMesh *linkToMesh;

	//World properities
	GLfloat LightAmbient[4]; 
	GLfloat LightDiffuse[4];
	GLfloat LightPosition[4];

	//Raycasting handling
	double fov;
	double frustrumSize;
	float nearClippingPlaneDistance;
	bool paintRay;

	Vector3 direction;
	Vector3 testProbe;
	vector<Vector4> pointRays;

	float distanceToCamera;


	double tempZoom;
};
