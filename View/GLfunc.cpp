#include "GLfunc.h"

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

void normalizeAngle(int &angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360 * 16)
		angle -= 360 * 16;
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
//------------------------------------------------------------
void paintCellTriangle(double& x1, double& y1, double& z1, double color1,
	double& x2, double& y2, double& z2, double color2,
	double& x3, double& y3, double& z3, double color3, int palette, const double& vmin, const double& vmax)
{
	GLfloat val;
	GLfloat color[] = { 0.f, 0.f, 0.f, 1.f };

	if (palette == 0)
	{
		//glNormal3d( (y2-y1)*(z3-z1) - (z2-z1)*(y3-y1), 
		//			(z2-z1)*(x3-x1) - (x2-x1)*(z3-z1), 
		//			(x2-x1)*(y3-y1) - (y2-y1)*(x3-x1) );
		val = color1;
		hotToColdMap(val, vmin, vmax, color[0], color[1], color[2]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
		glMaterialfv(GL_FRONT, GL_AMBIENT, color);
		//glNormal3d(x1, y1, z1);
		glVertex3f(x1, y1, z1);

		val = color2;
		hotToColdMap(val, vmin, vmax, color[0], color[1], color[2]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
		glMaterialfv(GL_FRONT, GL_AMBIENT, color);
		//glNormal3d(x2, y2, z2);
		glVertex3f(x2, y2, z2);

		val = color3;
		hotToColdMap(val, vmin, vmax, color[0], color[1], color[2]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
		glMaterialfv(GL_FRONT, GL_AMBIENT, color);
		//glNormal3d(x3, y3, z3);
		glVertex3f(x3, y3, z3);

	}
	else 
	{
		val = color1;
		grayMap(val, vmin, vmax, color[0], color[1], color[2]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
		glMaterialfv(GL_FRONT, GL_AMBIENT, color);
		//glNormal3d(x1, y1, z1);
		glVertex3f(x1, y1, z1);

		val = color2;
		grayMap(val, vmin, vmax, color[0], color[1], color[2]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
		glMaterialfv(GL_FRONT, GL_AMBIENT, color);
		//glNormal3d(x2, y2, z2);
		glVertex3f(x2, y2, z2);

		val = color3;
		grayMap(val, vmin, vmax, color[0], color[1], color[2]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
		glMaterialfv(GL_FRONT, GL_AMBIENT, color);
		//glNormal3d(x3, y3, z3);
		glVertex3f(x3, y3, z3);

	}
}
void paintTheRay(float x, float y, float z)
{
	//Multiply to achieve length 50
	x = x * 50;
	y = y * 50;
	z = z * 50;
	glBegin(GL_TRIANGLES);

	GLfloat col1[] = { 1.0, 1.0, 1.0, 1.f };

	glMaterialfv(GL_FRONT, GL_DIFFUSE, col1);
	glMaterialfv(GL_FRONT, GL_AMBIENT, col1);
	glNormal3d(0.02, 0.02, 0);
	glVertex3f(0.02, 0.02, 0.0);

	glNormal3d(x, y, z);
	glVertex3f(x, y, z);

	glNormal3d(0.03, 0, 0);
	glVertex3f(0.03, 0, 0);

	glEnd();

	glBegin(GL_TRIANGLES);

	//glNormal3d( (y2-y1)*(z3-z1) - (z2-z1)*(y3-y1), 
	//			(z2-z1)*(x3-x1) - (x2-x1)*(z3-z1), 
	//			(x2-x1)*(y3-y1) - (y2-y1)*(x3-x1) );

	glMaterialfv(GL_FRONT, GL_DIFFUSE, col1);
	glMaterialfv(GL_FRONT, GL_AMBIENT, col1);
	glNormal3d(0.02, -0.02, 0);
	glVertex3f(0.02, -0.02, 0.0);

	glNormal3d(x, y, z);
	glVertex3f(x, y, z);

	glNormal3d(0.03, 0, 0);
	glVertex3f(0.03, 0, 0);

	glEnd();

	glBegin(GL_TRIANGLES);

	//glNormal3d( (y2-y1)*(z3-z1) - (z2-z1)*(y3-y1), 
	//			(z2-z1)*(x3-x1) - (x2-x1)*(z3-z1), 
	//			(x2-x1)*(y3-y1) - (y2-y1)*(x3-x1) );

	glMaterialfv(GL_FRONT, GL_DIFFUSE, col1);
	glMaterialfv(GL_FRONT, GL_AMBIENT, col1);
	glNormal3d(-0.02, -0.02, 0);
	glVertex3f(-0.02, -0.02, 0.0);

	glNormal3d(x, y, z);
	glVertex3f(x, y, z);

	glNormal3d(0.03, 0, 0);
	glVertex3f(0.03, 0, 0);

	glEnd();

	glBegin(GL_TRIANGLES);

	//glNormal3d( (y2-y1)*(z3-z1) - (z2-z1)*(y3-y1), 
	//			(z2-z1)*(x3-x1) - (x2-x1)*(z3-z1), 
	//			(x2-x1)*(y3-y1) - (y2-y1)*(x3-x1) );

	glMaterialfv(GL_FRONT, GL_DIFFUSE, col1);
	glMaterialfv(GL_FRONT, GL_AMBIENT, col1);
	glNormal3d(-0.02, 0.02, 0);
	glVertex3f(-0.02, 0.02, 0.0);

	glNormal3d(x, y, z);
	glVertex3f(x, y, z);

	glNormal3d(0.03, 0, 0);
	glVertex3f(0.03, 0, 0);

	glEnd();

}
void paintCellTriangleFull(float x1, float y1, float z1, QColor color1,
	float x2, float y2, float z2, QColor color2,
	float x3, float y3, float z3, QColor color3)
{

	float f[1];
	glGetFloatv(GL_LINE_WIDTH, f);
	glLineWidth(1.0f);
	glLineStipple(1, (short)2);

	glBegin(GL_TRIANGLES);
	glColor3f(color1.redF(), color1.greenF(), color1.blueF());
	glVertex3f(x1, y1, z1);
	glColor3f(color2.redF(), color2.greenF(), color2.blueF());
	glVertex3f(x2, y2, z2);
	glColor3f(color3.redF(), color3.greenF(), color3.blueF());
	glVertex3f(x3, y3, z3);
	glEnd();


	glLineWidth(f[0]);

}
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

//{
//		float f[1];
//		glGetFloatv(GL_LINE_WIDTH, f);
//		glLineWidth(3.0f);
//		glBegin(GL_LINES);
//			glColor3f(1.0f, 0.0f, 0.0f);
//			glVertex3f(0.0f, 0.0f, 0.0f);
//			glVertex3f(0.3f, 0.0f, 0.0f);
//			glColor3f(0.0f,1.0f,0.0f);
//			glVertex3f(0.0f, 0.0f, 0.0f);
//			glVertex3f(0.0f, 0.3f, 0.0f);
//		glEnd();
//		glLineWidth(f[0]);
//	}

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


bool invertMatrix(const GLfloat m[16], double invOut[16])
{
	double inv[16], det;
	int i;

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return false;

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		invOut[i] = inv[i] * det;

	return true;
}

void drawSphere(double r, int lats, int longs, double xorg, double yorg, double zorg, float rr, float gg, float bb) {
	int i, j;
	for (i = 0; i <= lats; i++) {
		double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
		double z0 = r* sin(lat0);
		double zr0 = r*  cos(lat0);

		double lat1 = M_PI * (-0.5 + (double)i / lats);
		double z1 = r*  sin(lat1);
		double zr1 = r*  cos(lat1);
		//			(x2-x1)*(y3-y1) - (y2-y1)*(x3-x1) );
		GLfloat col1[] = { rr, gg, bb, 1.f };

		glMaterialfv(GL_FRONT, GL_DIFFUSE, col1);
		glMaterialfv(GL_FRONT, GL_AMBIENT, col1);

		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= longs; j++) {
			double lng = 2 * M_PI * (double)(j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3f(x * zr0 + xorg, y * zr0 + yorg, z0 + zorg);
			glVertex3f(x * zr0 + xorg, y * zr0 + yorg, z0 + zorg);
			glNormal3f(x * zr1*xorg, y * zr1 + yorg, z1 + zorg);
			glVertex3f(x * zr1 + xorg, y * zr1 + yorg, z1 + zorg);
		}
		glEnd();
	}
}



void hotToColdMap(GLfloat &v, const GLfloat &vmin, const GLfloat &vmax, GLfloat &rr, GLfloat &gg, GLfloat &bb)
{
	GLfloat dv;

	if (v < vmin)
		v = vmin;
	if (v > vmax)
		v = vmax;
	dv = vmax - vmin;

	if (v < (vmin + 0.25 * dv)) {
		rr = 0;
		gg = 4 * (v - vmin) / dv;
		bb = 1.0;
	}
	else if (v < (vmin + 0.5 * dv)) {
		rr = 0;
		gg = 1.0;
		bb = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
	}
	else if (v < (vmin + 0.75 * dv)) {
		rr = 4 * (v - vmin - 0.5 * dv) / dv;
		gg = 1.0;
		bb = 0;
	}
	else {
		rr = 1.0;
		gg = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
		bb = 0;
	}


}
void hotNlMap(GLfloat &v, const GLfloat &vmin, const GLfloat &vmax, GLfloat &rr, GLfloat &gg, GLfloat &bb)
{
	GLfloat dv;

	if (v < vmin)
		v = vmin;
	if (v > vmax)
		v = vmax;
	dv = vmax - vmin;

	if (v < (vmin + 0.02 * dv)) {
		rr = 15 * (v - vmin) / dv;
		gg = 0;
		bb = 0;
	}
	else if (v < (vmin + 0.3 * dv)) {
		rr = 2.5* (v - vmin + 0.1 * dv) / dv;
		gg = 0;
		bb = 0;
	}
	else if (v < (vmin + 0.7 * dv)) {
		rr = 1.0;
		gg = 2.5 * (v - vmin - 0.3 * dv) / dv;
		bb = 0;
	}
	else {
		gg = 1.0;
		rr = 1.0;
		bb = 3.33 * (v - vmin - 0.7 * dv) / dv;
	}
}

void hotMap(GLfloat &v, const GLfloat &vmin, const GLfloat &vmax, GLfloat &rr, GLfloat &gg, GLfloat &bb)
{
	GLfloat dv;

	if (v < vmin)
		v = vmin;
	if (v > vmax)
		v = vmax;
	dv = vmax - vmin;

	if (v < (vmin + 0.4 * dv)) {
		rr = 2.5 * (v - vmin ) / dv;
		gg = 0;
		bb = 0;
	}
	else if (v < (vmin + 0.8 * dv)) {
		rr = 1.0;
		gg = 2.5 * (v - vmin - 0.4 * dv) / dv;
		bb = 0;
	}
	else {
		gg = 1.0;
		rr = 1.0;
		bb = 2.5 * (v - vmin - 0.8 * dv) / dv;
	}
}

void coldMap(GLfloat &v, const GLfloat &vmin, const GLfloat &vmax, GLfloat &rr, GLfloat &gg, GLfloat &bb)
{
	GLfloat dv;

	if (v < vmin)
		v = vmin;
	if (v > vmax)
		v = vmax;
	dv = vmax - vmin;

	if (v < (vmin + 0.4 * dv)) {
		rr = 0.0;
		gg = 0;
		bb = 2.5 * (v - vmin) / dv;
	}
	else if (v < (vmin + 0.8 * dv)) {
		rr = 0.0;
		gg = 2.5 * (v - vmin - 0.4 * dv) / dv;
		bb = 1.0;
	}
	else {
		rr = 2.5 * (v - vmin - 0.8 * dv) / dv;
		gg = 1.0;
		bb = 1.0;
	}
}
//----------------------------------------------------------------------------------------
void grayMap(GLfloat &v, const GLfloat &vmin, const GLfloat &vmax, GLfloat &rr, GLfloat &gg, GLfloat &bb)
{
	GLfloat dv;

	if (v < vmin)
		v = vmin;
	if (v > vmax)
		v = vmax;
	dv = vmax - vmin;

	rr = (v - vmin) / dv;
	gg = (v - vmin) / dv;
	bb = (v - vmin) / dv;
	
}

Vector3 get_arcball_vector(double width, double height, int x, int y)
{

	Vector3 center = Vector3(width / 2, height / 2, 0);
	//Vector3 P = GLKVector3Subtract(touchPoint, center);
	float radius = width / 3;
	//view->directionRay = view->screenToWorld(x, y, view->width(), view->height());

	// Flip the y-axis because pixel coords increase toward the bottom.
	Vector3 P = Vector3(x, y, 0) - center;
	P = Vector3(-P.x, -P.y, 0);

	float radius2 = radius * radius;
	float length2 = P.x*P.x + P.y*P.y;

	if (length2 <= radius2)
		P.z = sqrt(radius2 - length2);
	else
	{
		P.x *= radius / sqrt(length2);
		P.y *= radius / sqrt(length2);
		P.z = 0;
/*
		P.z = radius2 / (2.0 * sqrt(length2));
		float length = sqrt(length2 + P.z * P.z);
		P = (P / length);*/
	}

	return P.normalize();
}

GLfloat* quaternionToMatrix(QQuaternion q, GLfloat* m)
{
	q.normalize();
	float qw = q.scalar();
	float qx = q.x();
	float qy = q.y();
	float qz = q.z();
	m[0] = 1.0f - 2.0f*qy*qy - 2.0f*qz*qz;
	m[1] = 2.0f*qx*qy - 2.0f*qz*qw;
	m[2] = 2.0f*qx*qz + 2.0f*qy*qw;
	m[3] = 0.0f;
	m[4] = 2.0f*qx*qy + 2.0f*qz*qw;
	m[5] = 1.0f - 2.0f*qx*qx - 2.0f*qz*qz;
	m[6] = 2.0f*qy*qz - 2.0f*qx*qw;
	m[7] = 0.0f;
	m[8] = 2.0f*qx*qz - 2.0f*qy*qw;
	m[9] = 2.0f*qy*qz + 2.0f*qx*qw;
	m[10] = 1.0f - 2.0f*qx*qx - 2.0f*qy*qy;
	m[11] = 0.0f;
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;

	GLfloat z[] = { 1.0f - 2.0f*qy*qy - 2.0f*qz*qz, 2.0f*qx*qy - 2.0f*qz*qw, 2.0f*qx*qz + 2.0f*qy*qw, 0.0f,
		2.0f*qx*qy + 2.0f*qz*qw, 1.0f - 2.0f*qx*qx - 2.0f*qz*qz, 2.0f*qy*qz - 2.0f*qx*qw, 0.0f,
		2.0f*qx*qz - 2.0f*qy*qw, 2.0f*qy*qz + 2.0f*qx*qw, 1.0f - 2.0f*qx*qx - 2.0f*qy*qy, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
	m = z;
	return m;
}