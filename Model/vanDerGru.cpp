#include "vanDerGru.h"
#include <cmath>

//HORMONAL ACTIVITY ADDED ARTIFICIALLY!!!
//------------------------------------------------------------
vanDerGru::vanDerGru(void):
	Oscillator()
{
	m_alpha = 0.1;
	m_e = 4.3;
	m_d = 3.0;
	m_f = 0.00005;
	m_v1 = 1;//1.0
	m_v2 = -1;//1.0
	//xmax = 1.56; 
	//xmin = -2.4;
	//type3d =0;
	m_v_current.clear();//V, 0
	m_v_current.push_back(0.0);//V, 0

	m_vdPtype = 2;

	vzero = 0.5;
	vmax = 1;////1.3;
	vmin = 0.5;//-0.5;
}
vanDerGru::~vanDerGru(void)
{
}
//------------------------------------------------------------
double vanDerGru::vanDerGruFunction(double potential, double current)
{
	double func = 0;
	if(m_vdPtype == 1)
	{
		func = (-1) * m_alpha * ( potential - m_v2) * ( potential - m_v1 ) * current;
		func -= m_f * potential * ( potential + m_d ) * ( potential + m_e);
		func+= m_hormonalActivity;
	}
	else
	{
		func = (m_f) *(( potential * ( potential + m_d ) * ( potential + m_e)) - 0 )  ;
		func+= m_hormonalActivity;
	}
	return func;
}
//--------------------------------------------------------------
double vanDerGru::getPotentialPrim()
{
	//if( m_drive*m_drive > 0.5* m_v_current*m_v_current) 
	//{
	//	if(m_drive* m_v_current <0 ) return 0.5*m_v_current;
	//	if(m_drive* m_v_current >0 ) return 1.5*m_v_current;
	//}
	//else
	//{
	/*if( abs(abs(potential)-abs(m_v1))<=0.01 ||  abs(abs(potential)-abs(m_v2))<=0.01)
	{
		return m_v_current;
	}
		return m_v_current + m_drive;*/
	if(m_vdPtype == 1)
	{
		return m_v_current[0];
	}
	else
	{
		double func = 0;
		func = (m_alpha) * ( m_v1*(m_v_potential-vzero)/(vmax-vmin) - (1.0/3.0) * pow( (m_v_potential-vzero)/(vmax-vmin), 3) - m_v_current[0] );
		//func = (m_alpha) * ( 0.6-pow(potential - 0.7,4) * log( potential+2.2 ) - m_v_current ) + m_drive ;
		return func;
	}
}
//-------------------------------------------------------------
double vanDerGru::getCurrentPrim(int which)
{
	return vanDerGruFunction( (m_v_potential-vzero)/(vmax-vmin), m_v_current[0]);
}

void vanDerGru::setDefaultAvParameters()
{
	m_alpha = 0.1;
	m_e = 3.3;
	m_d = 3.0;
	m_f = 0.00005;
	m_v1 = 1;//1.0
	m_v2 = -1;//1.0
}
//-------------------------------------------------------------
void vanDerGru::setParameter(double value, OSC_PARAMETER parameter)
{	
	if(parameter == VDG_ALPHA)
	{	
		m_alpha = value;
	}
	else if(parameter == VDG_V1)
	{	
		m_v1 = value;
	}
	else if(parameter == VDG_V2)
	{	
		m_v2 = value;
	}
	else if(parameter == VDG_E)
	{	
		m_e = value;
	}
	else if(parameter == VDG_F)
	{	
		m_f = value;
	}
	else if(parameter == VDG_D)
	{	
		m_d = value;
	}
	{

	}
}
//-----------
void vanDerGru::setValueAlpha(double value) 
{ 
	//if( parameter != value)
	//{
	//	*parameter = value; 
	//}
	m_alpha = value;
	//setParameter(&m_alpha,value);
}
void vanDerGru::setValueF(double value) 
{
	//setParameter(&m_f,value);
}
void vanDerGru::setValueD(double value) 
{ 
	//setParameter(&m_d,value);
}
void vanDerGru::setValueE(double value) 
{ 
	//setParameter(&m_e,value);
}
void vanDerGru::setValueV1(double value) 
{ 
	//setParameter(&m_v1,value);
}
void vanDerGru::setValueV2(double value) 
{ 
	//setParameter(&m_v2,value);
}