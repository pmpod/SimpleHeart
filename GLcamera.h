#ifndef _CAMERA_H
#define _CAMERA_H

//#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "glu32.lib")
//#pragma comment(lib, "glaux.lib")


//#include <gl\gl.h>						// Header File For The OpenGL32 Library
//#include <gl\glu.h>						// Header File For The GLU32 Library
//#include <gl\glaux.h>					// Header File For The Glaux Library
#include <QGLWidget>

#define CAMERASPEED	0.1f				// The Camera Speed



typedef struct tVector3					// expanded 3D vector struct
{			
	tVector3() {}	// constructor
	tVector3 (float new_x, float new_y, float new_z) // initialize constructor	 
	{
		x = new_x; 
		y = new_y; 
		z = new_z;
	}
	tVector3 operator+(tVector3 vVector) {return tVector3(vVector.x+x, vVector.y+y, vVector.z+z);}
	tVector3 operator-(tVector3 vVector) {return tVector3(x-vVector.x, y-vVector.y, z-vVector.z);}
	tVector3 operator*(float number)	 {return tVector3(x*number, y*number, z*number);}
    tVector3 operator^(tVector3 vVector)	 
	{
		return tVector3(y*vVector.z-z*vVector.y, x*vVector.z-z*vVector.x, x*vVector.y-y*vVector.x);
	}
	tVector3 operator/(float number)	 {return tVector3(x/number, y/number, z/number);}
	
	float x, y, z;						// 3D vector coordinates

}tVector3;


class CCamera 
{
	public:

		tVector3 mPos;		// camera position
		tVector3 mView;		// camera view(target)
		tVector3 mUp;		// camera upvector(tilt)

		void Move_Camera(float speed);
		void Raise_Camera(float speed);
		void Strafe_Camera(float speed);
		void StrafeView_Camera(float speed);
		void Position_Camera(float pos_x,  float pos_y,  float pos_z,
			 				 float view_x, float view_y, float view_z,
							 float up_x,   float up_y,   float up_z);
};

#endif