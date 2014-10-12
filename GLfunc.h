#ifndef _GLFUNC_H
#define _GLFUNC_H

#include "GLcamera.h"
#include <QtGui>
void paintGround(float deltaX,float deltaY, int CellsX, int CellsY);
void paintOrigin();
//void initGL(FXGLCanvas *glcanvas, FXGLVisual *glvisual );
void rotateGL(int coord, int angle_X, int angle_Y, int angle_Z );
void paintCell(int CellsX, int CellsY, float deltaX,float deltaY, float coloura,float colourb,float colourc);
void paintProbe(int CellsX, int CellsY, float deltaX,float deltaY, float coloura,float colourb,float colourc);
void paintCellPoint(int CellsX, int CellsY, float deltaX,float deltaY, 
					float colour1, float colour2,float colour3,float colour4, int palette);
void paintPixel(int CellsX, int CellsY, float deltaX,float deltaY, QColor colour);
void paintPixelB(int CellsX, int CellsY, float deltaX,float deltaY, QColor colour);
void HSVtoRGB(double h, double s, double v, double* r, double* g, double* b);
/*******************************************************************************/
#endif
