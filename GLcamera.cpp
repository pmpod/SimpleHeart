#include "GLcamera.h"


//--------------------------------------------------------------------------
void CCamera::Position_Camera(float pos_x,  float pos_y,  float pos_z,
							  float view_x, float view_y, float view_z,
							  float up_x,   float up_y,   float up_z)
{
	mPos	= tVector3(pos_x,  pos_y,  pos_z ); // set position
	mView	= tVector3(view_x, view_y, view_z); // set view
	mUp		= tVector3(up_x,   up_y,   up_z  ); // set the up vector					
}


void CCamera::Move_Camera(float speed)
{
	tVector3 vVector = mView - mPos;	// Get the view vector
	
	mPos.x  = mPos.x  + vVector.x * speed *2;
	mPos.z  = mPos.z  + vVector.z * speed *2;
	mView.x = mView.x + vVector.x * speed *2;
	mView.z = mView.z + vVector.z * speed *2;
	//mPos.x  = mPos.x  + speed;
	//mPos.z  = mPos.z  + speed;
	//mView.x = mView.x + speed;
	//mView.z = mView.z + speed;
}

void CCamera::Raise_Camera(float speed)
{
	tVector3 vVector = mView - mPos;	// Get the view vector
	
	mPos.y  = mPos.y  + speed *2;
	///mPos.z  = mPos.z  + vVector.z * speed;
	mView.y = mView.y + speed *2;
	//mView.z = mView.z + vVector.z * speed;
}

void CCamera::Strafe_Camera(float speed)
{
	tVector3 vVector = mView - mPos;	// Get the view vector
	tVector3 yVector(0,1,0);	// Get the view vector
	tVector3 XVector = vVector^yVector;

	mPos.x  = mPos.x  + XVector.x * speed *2;
	mPos.z  = mPos.z  + XVector.z * speed *2;
	//mView.x = mView.x + XVector.x * speed;
	//mView.z = mView.z + XVector.z * speed;
}
void CCamera::StrafeView_Camera(float speed)
{
	tVector3 vVector = mView - mPos;	// Get the view vector
	tVector3 yVector(0,1,0);	// Get the view vector
	tVector3 XVector = vVector^yVector;

	mPos.x  = mPos.x  + XVector.x * speed *2;
	mPos.z  = mPos.z  + XVector.z * speed *2;
	mView.x = mView.x + XVector.x * speed *2;
	mView.z = mView.z + XVector.z * speed *2;
}