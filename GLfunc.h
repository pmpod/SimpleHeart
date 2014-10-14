#ifndef _GLFUNC_H
#define _GLFUNC_H

#include "GLcamera.h"
#include <QtGui>
void paintGround(float deltaX,float deltaY, int CellsX, int CellsY);
void paintOrigin(float frustrumSize, float scale);
void normalizeAngle(int &angle);
//void initGL(FXGLCanvas *glcanvas, FXGLVisual *glvisual );
void rotateGL(int coord, int angle_X, int angle_Y, int angle_Z );
void paintCell(int CellsX, int CellsY, float deltaX,float deltaY, float coloura,float colourb,float colourc);
void paintProbe(int CellsX, int CellsY, float deltaX,float deltaY, float coloura,float colourb,float colourc);
void paintCellPoint(int CellsX, int CellsY, float deltaX,float deltaY, 
					float colour1, float colour2,float colour3,float colour4, int palette);
void paintCellTriangle(float x1, float y1, float z1, float color1,
	float x2, float y2, float z2, float color2,
	float x3, float y3, float z3, float color3, int palette);
void paintCellTriangleFull(float x1, float y1, float z1, QColor color1,
	float x2, float y2, float z2, QColor color2,
	float x3, float y3, float z3, QColor color3);
void paintPixel(int CellsX, int CellsY, float deltaX,float deltaY, QColor colour);
void paintPixelB(int CellsX, int CellsY, float deltaX,float deltaY, QColor colour);
void HSVtoRGB(double h, double s, double v, double* r, double* g, double* b);
void paintTheRay(float x, float y, float z);
bool invertMatrix(const GLfloat m[16], double invOut[16]);
void drawSphere(double r, int lats, int longs, double xorg, double yorg, double zorg, float rr, float gg, float bb);
/*******************************************************************************/
#endif
