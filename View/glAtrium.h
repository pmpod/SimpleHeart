#pragma once
#include <QGLWidget>
#include "AtrialMachine2d.h"
#include "GLcamera.h"
#include "GLfunc.h"
#include "DiffusionMatrix.h"
#include "Model\CardiacMesh.h"
#include "Support\Vectors.h"
#include "Support\Matrices.h"
#include "View\SimViewStateView.h"
#include "View\SimViewStateEP.h"
#include "View\SimViewStateStructure.h"
#include "View\SimViewStateDiffusion.h"
///TODO Update to OpenGL 3.0

class glAtrium: public QGLWidget
{
	Q_OBJECT

public:
	glAtrium(CardiacMesh *linkMesh, AtrialMachine2d *link, QWidget *parent = 0);
	~glAtrium(void);
	
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

	void setLastPos(const QPoint& pos);
	CardiacMesh *linkToMesh;		///Link to the Model

	
	double paintValueDiffusion;
	double paintValueERP;
private:

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
	void setZRotation(int angle);
	void setTop();
	void setSide1();
	void setSide2();
	void setStateStructureModifier(bool b);
	void setStateDiffusionModifier(bool b);
	void setStateViewer(bool b);
	void setStateEP(bool b);
	void setPaintConductivity(bool b);
	void setPaintERP(bool b);
	void setPaletteHSV();
	void setPaletteGray();
	void setDisplayPotential(bool b);
	void setDisplayCSD(bool b);
	void setDisplayCurrent1(bool b);
	void setDisplayCurrent2(bool b);
	void setDisplayActivationTime(bool b);
	void setDisplayPPIRTCL(bool b);
	void displayElectrogram();
	void setOutlineUniform(bool b);
	void setOutlineGauss(bool b);
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

	Vector3 screenToWorld(double x, double y, double width, double height);
	int itemAt(double xx, double yy, double zz);
	void paintOrigin(float scale);

//! [2]

//! [3]
private:
	AtrialMachine2d *linkToMachine;		///Link to the Controller


	GLuint displayListIndex;
	//Model view properties:
	float viewWidth;
	float viewHeight;
	GLfloat m[16];
	Matrix4 modelMatrix;
	Matrix4 modelRotation;

	float distanceToCamera;
	int m_palette;
	float rotationSpeed;
	float zoomingSpeed;
    int xRot;
    int yRot;
	int zRot;
	int gridSize;

	//Event handling properties
	bool m_controlable;
	int workMode;
    QPoint lastPos;


	//World properities
	QColor backgroundColor;
	GLfloat LightAmbient[4]; 
	GLfloat LightDiffuse[4];
	GLfloat LightPosition[4];

	//Raycasting handling
	vector<Vector4> pointRays;
	Vector3 directionRay;
	float fov;
	float frustrumSize;
	float nearClippingPlaneDistance;
	float farClippingPlaneDistance;

	//Debug handling
	bool paintRay;
	Vector3 testProbe;

private:
	friend class SimViewState;
	friend class SimViewStateView;
	friend class SimViewStateStructure;
	friend class SimViewStateDiffusion;
	friend class SimViewStateEP;
	void ChangeState(SimViewState*);
	SimViewState* _state;
};
