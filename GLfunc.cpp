#include "GLfunc.h"
#include <cmath>

const int COORD_X = 1;
const int COORD_Y = 2;
const int COORD_Z = 3;
  
void paintGround(float deltaX,float deltaY, int CellsX, int CellsY) {

		float f[1];
		glGetFloatv(GL_LINE_WIDTH, f);
		glLineWidth(1.0f);
		glLineStipple(1, (short)2);
		glBegin(GL_LINES);
		glColor3f(0.2f, 0.2f, 0.2f);
			for(int i=0;i<CellsX+1;++i){
				glVertex2f(i*deltaX, 0.0f);
				glVertex2f(i*deltaX, deltaY*CellsY);
			}
			for(int i=0;i<CellsY+1;++i){
				glVertex2f(0.0f,i*deltaY);
				glVertex2f(deltaX*CellsX,i*deltaY);
			}
		glEnd();
		glLineWidth(f[0]);
	}

void paintCell(int CellsX, int CellsY, float deltaX,float deltaY,float coloura,float colourb,float colourc) {

		float f[1];
		glGetFloatv(GL_LINE_WIDTH, f);
		glLineWidth(1.0f);
		glLineStipple(1, (short)2);
		double rr = coloura;
		double gg = colourb;
		double bb = colourc;
		//HSVtoRGB(coloura,1.0,1.0,&rr,&gg,&bb);
		glColor3f(rr, gg, bb);
		glBegin(GL_QUADS);
				glVertex3f(CellsX*deltaX, deltaY*CellsY, -0.03f);
				glVertex3f(CellsX*deltaX, deltaY*(CellsY+1), -0.03f);
				glVertex3f((CellsX+1)*deltaX, deltaY*(CellsY+1), -0.03f);
				glVertex3f((CellsX+1)*deltaX, deltaY*CellsY, -0.03f);

				glVertex3f((CellsX+1)*deltaX, deltaY*(CellsY+1), -0.03f);
				glVertex3f((CellsX+1)*deltaX, deltaY*CellsY, -0.03f);
				glVertex3f((CellsX+1)*deltaX, deltaY*CellsY, 0.2f);
				glVertex3f((CellsX+1)*deltaX, deltaY*(CellsY+1), 0.2f);

				glVertex3f(CellsX*deltaX, deltaY*CellsY, -0.03f);
				glVertex3f(CellsX*deltaX, deltaY*(CellsY+1), -0.03f);
				glVertex3f(CellsX*deltaX, deltaY*(CellsY+1), 0.2f);
				glVertex3f(CellsX*deltaX, deltaY*CellsY, 0.2f);

				glVertex3f(CellsX*deltaX, deltaY*(CellsY+1), -0.03f);
				glVertex3f((CellsX+1)*deltaX, deltaY*(CellsY+1), -0.03f);
				glVertex3f((CellsX+1)*deltaX, deltaY*(CellsY+1), 0.2f);
				glVertex3f(CellsX*deltaX, deltaY*(CellsY+1), 0.2f);

				glVertex3f(CellsX*deltaX, deltaY*CellsY, -0.03f);
				glVertex3f((CellsX+1)*deltaX, deltaY*CellsY, -0.03f);
				glVertex3f((CellsX+1)*deltaX, deltaY*CellsY, 0.2f);
				glVertex3f(CellsX*deltaX, deltaY*CellsY, 0.2f);

				glVertex3f(CellsX*deltaX, deltaY*CellsY, 0.2f);
				glVertex3f(CellsX*deltaX, deltaY*(CellsY+1), 0.2f);
				glVertex3f((CellsX+1)*deltaX, deltaY*(CellsY+1), 0.2f);
				glVertex3f((CellsX+1)*deltaX, deltaY*CellsY, 0.2f);
		glEnd();
		glLineWidth(f[0]);
	}


void paintProbe(int CellsX, int CellsY, float deltaX,float deltaY,float coloura,float colourb,float colourc) {

		float f[1];
		glGetFloatv(GL_LINE_WIDTH, f);
		glLineWidth(1.0f);
		glLineStipple(1, (short)2);
		double rr = coloura;
		double gg = colourb;
		double bb = colourc;
		//HSVtoRGB(coloura,1.0,1.0,&rr,&gg,&bb);
		glColor3f(rr, gg, bb);

		glBegin(GL_TRIANGLES);
		
		glColor3f(0, 0, 0);
				glVertex3f((CellsX+0.5)*deltaX, deltaY*(CellsY+0.5), 0.06f);
		glColor3f(rr, gg, bb);
				glVertex3f((CellsX+2)*deltaX, deltaY*(CellsY+1.3), 0.06f);
				glVertex3f((CellsX+1.3)*deltaX, deltaY*(CellsY+2), 0.06f);
				
		glColor3f(0, 0, 0);
				glVertex3f((CellsX+0.5)*deltaX, deltaY*(CellsY+0.5), 0.06f);
		glColor3f(rr, gg, bb);
				glVertex3f((CellsX+2)*deltaX, deltaY*(CellsY-0.3), 0.06f);
				glVertex3f((CellsX+1.3)*deltaX, deltaY*(CellsY-1), 0.06f);
				
		glColor3f(0, 0, 0);
				glVertex3f((CellsX+0.5)*deltaX, deltaY*(CellsY+0.5), 0.06f);
		glColor3f(rr, gg, bb);
				glVertex3f((CellsX-1.0)*deltaX, deltaY*(CellsY+1.3), 0.06f);
				glVertex3f((CellsX-0.3)*deltaX, deltaY*(CellsY+2), 0.06f);
				
		glColor3f(0, 0, 0);
				glVertex3f((CellsX+0.5)*deltaX, deltaY*(CellsY+0.5), 0.06f);
		glColor3f(rr, gg, bb);
				glVertex3f((CellsX-1)*deltaX, deltaY*(CellsY-0.3), 0.06f);
				glVertex3f((CellsX-0.3)*deltaX, deltaY*(CellsY-1), 0.06f);
	glEnd();
		glLineWidth(f[0]);
	}
//------------------------------------------------------------
void paintCellPoint(int CellsX, int CellsY, float deltaX,float deltaY, 
					float colour1, float colour2,float colour3,float colour4,  int palette)
{

		float f[1];
		glGetFloatv(GL_LINE_WIDTH, f);
		glLineWidth(1.0f);
		glLineStipple(1, (short)2);

		double rr = colour1;
		double gg = colour1;
		double bb = colour1;

		if(palette == 0 )
		{
			glBegin(GL_QUADS);
						HSVtoRGB(1-sqrt(colour1),1.0,0.2+0.8*colour1,&rr,&gg,&bb);
						glColor3f(rr, gg, bb);
					glVertex3f(CellsX*deltaX, deltaY*CellsY, colour1/10.0);
						HSVtoRGB(1-sqrt(colour1),1.0,0.2+0.8*colour1,&rr,&gg,&bb);
						glColor3f(rr, gg, bb);
					glVertex3f(CellsX*deltaX, deltaY*(CellsY+1),colour1/10.0);
						HSVtoRGB(1-sqrt(colour1),1.0,0.2+0.8*colour1,&rr,&gg,&bb);
						glColor3f(rr, gg, bb);
					glVertex3f((CellsX+1)*deltaX, deltaY*(CellsY+1), colour1/10.0);
						HSVtoRGB(1-sqrt(colour1),1.0,0.2+0.8*colour1,&rr,&gg,&bb);
						glColor3f(rr, gg, bb);
					glVertex3f((CellsX+1)*deltaX, deltaY*CellsY, colour1/10.0);
			glEnd();
		}
		else
		{
			glColor3f(rr, rr, rr);

		//glColor3f(colour1, colour1, colour1);
			glBegin(GL_QUADS);
				glVertex3f(CellsX*deltaX, deltaY*CellsY, colour1/10.0);
					//HSVtoRGB(1-sqrt(colour2),1.0,0.2+0.8*colour2,&rr,&gg,&bb);
					//glColor3f(rr, gg, bb);
				glVertex3f(CellsX*deltaX, deltaY*(CellsY+1),colour1/10.0);
					//HSVtoRGB(1-sqrt(colour3),1.0,0.2+0.8*colour3,&rr,&gg,&bb);
					//glColor3f(rr, gg, bb);
				glVertex3f((CellsX+1)*deltaX, deltaY*(CellsY+1), colour1/10.0);
					//HSVtoRGB(1-sqrt(colour4),1.0,0.2+0.8*colour4,&rr,&gg,&bb);
					//glColor3f(rr, gg, bb);
				glVertex3f((CellsX+1)*deltaX, deltaY*CellsY, colour1/10.0);
			glEnd();
		}
		glLineWidth(f[0]);
	}

void paintPixel(int CellsX, int CellsY, float deltaX,float deltaY, QColor colour)
{

		float f[1];
		glGetFloatv(GL_LINE_WIDTH, f);
		glLineWidth(1.0f);
		glLineStipple(1, (short)2);

		double rr = colour.redF();
		double gg = colour.greenF();
		double bb = colour.blueF();
		glColor3f(rr, gg, bb);

		glBegin(GL_QUADS);
				glVertex3f(CellsX*deltaX, deltaY*CellsY, 0);
				glVertex3f((CellsX+1)*deltaX, deltaY*CellsY, 0);
				glVertex3f((CellsX+1)*deltaX, deltaY*(CellsY+1), 0);
				glVertex3f(CellsX*deltaX, deltaY*(CellsY+1),0);


		glEnd();
		glLineWidth(f[0]);


}

void paintPixelB(int CellsX, int CellsY, float deltaX,float deltaY, QColor colour)
{

		float f[1];
		glGetFloatv(GL_LINE_WIDTH, f);
		glLineWidth(1.0f);
		glLineStipple(1, (short)2);

		double rr = colour.redF();
		double gg = colour.greenF();
		double bb = colour.blueF();
		glColor3f(rr, gg, bb);

		glBegin(GL_QUADS);
				glVertex3f(CellsX*deltaX, deltaY*CellsY, -0.01f);
				glVertex3f((CellsX+1)*deltaX, deltaY*CellsY, -0.01f);
				glVertex3f((CellsX+1)*deltaX, deltaY*(CellsY+1), -0.01f);
				glVertex3f(CellsX*deltaX, deltaY*(CellsY+1),-0.1f);


		glEnd();
		glLineWidth(f[0]);


}
//------------------------------------------------------------------------
void paintOrigin() {
		float f[1];
		glGetFloatv(GL_LINE_WIDTH, f);
		glLineWidth(3.0f);
		glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.3f, 0.0f, 0.0f);
			glColor3f(0.0f,1.0f,0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.3f, 0.0f);
		glEnd();
		glLineWidth(f[0]);
	}

//------------------------------------------------------------------------
//void initGL(FXGLCanvas *glcanvas, FXGLVisual *glvisual)
//{
//  GLdouble width = glcanvas->getWidth();
//  GLdouble height = glcanvas->getHeight();
//  GLdouble aspect = height>0 ? width/height : 1.0;
//  // Make context current
//  glcanvas->makeCurrent();
//
//  glViewport(0,0,glcanvas->getWidth(),glcanvas->getHeight());
//	
//
//  glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
//  glLoadIdentity();									// Reset The Projection Matrix
//  // Calculate The Aspect Ratio Of The Window
//  gluPerspective(45.0f,aspect,0.1f,100.0f);
//
//  glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
//  glLoadIdentity();									// Reset The Modelview Matrix
//      // Swap if it is double-buffered
//  glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
//	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
//	glClearDepth(1.0f);									// Depth Buffer Setup
//	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
//	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
//
//	if(glvisual->isDoubleBuffer()){
//    glcanvas->swapBuffers();
//    }
//
//
//  				           // Position      View		  Up(vector)
//
//
//
//  glcanvas->makeNonCurrent();
//}


void rotateGL(int coord, int angle_X, int angle_Y, int angle_Z )
{
	if(coord == COORD_X)
	{						
		glRotatef(angle_X,1.0f,0.0f,0.0f);
		glRotatef(angle_Y,0.0f,1.0f,0.0f);						
		glRotatef(angle_Z,0.0f,0.0f,1.0f);	
	}
	else if(coord == COORD_Y)
	{											
		glRotatef(angle_X,1.0f,0.0f,0.0f);
		glRotatef(angle_Y,0.0f,1.0f,0.0f);						
		glRotatef(angle_Z,0.0f,0.0f,1.0f);	
	}
	else
	{		
		glRotatef(angle_X,1.0f,0.0f,0.0f);
		glRotatef(angle_Y,0.0f,1.0f,0.0f);						
		glRotatef(angle_Z,0.0f,0.0f,1.0f);	

	}
}


void HSVtoRGB(double h, double s, double v, double* r, double* g, double* b)
{
  if ( s == 0 )
  {
     *r = v;
     *g = v;
     *b = v;
  }
  else
  {
     double var_h = h * 6;
     double var_i = floor( var_h );
     double var_1 = v * ( 1 - s );
     double var_2 = v * ( 1 - s * ( var_h - var_i ) );
     double var_3 = v * ( 1 - s * ( 1 - ( var_h - var_i ) ) );

     if      ( var_i == 0 ) { *r = v     ; *g = var_3 ; *b = var_1; }
     else if ( var_i == 1 ) { *r = var_2 ; *g = v     ; *b = var_1; }
     else if ( var_i == 2 ) { *r = var_1 ; *g = v     ; *b = var_3; }
     else if ( var_i == 3 ) { *r = var_1 ; *g = var_2 ; *b = v;     }
     else if ( var_i == 4 ) { *r = var_3 ; *g = var_1 ; *b = v;     }
     else                   { *r = v     ; *g = var_1 ; *b = var_2; }
  }
}

