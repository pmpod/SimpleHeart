#include "vanDerGru.h"
#include <cmath>

//HORMONAL ACTIVITY ADDED ARTIFICIALLY!!!
//------------------------------------------------------------
vanDerGru::vanDerGru(void):
	Oscillator()
{
	create(SA_NODE);
}

vanDerGru::vanDerGru(CELL_TYPE type)
{
	create(type);
}

void vanDerGru::create(CELL_TYPE type)
{
	m_type = type;
	m_vdPtype = 2;

	m_alpha = 0.1;
	m_e = 4.3;
	m_d = 3.0;
	m_f = 0.001;
	m_v1 = 1;//1.0
	m_v2 = -1;//1.0
	//xmax = 1.56; 
	//xmin = -2.4;
	//type3d =0;

	if (m_type == SA_NODE)
	{
		m_alpha = 0.1;
		m_e = 2.8;
		m_d = 4.0;
		m_f = 0.001;
		m_v1 = 1;//1.0
		m_v2 = -1;//1.0
	}
	else
	{
		m_alpha = 0.1;
		m_e = 2.8;
		m_d = 3.5;
		m_f = 0.001;
		m_v1 = 1;//1.0
		m_v2 = -1;//1.0
	}
	vzero = -25; //[mV]
	vmax = 5.0;  //[mV]
	vmin = -25;  //[mV]

	m_v_current.clear();
	m_v_current.push_back(0.0);//V, 0
	m_currentPRIM.clear();
	m_currentPRIM.push_back(0.0);
	m_currentTime = 0.0;
	m_previousTime = -0.1;


	m_v_potential = 0;
	m_previous_potential = 0;
	m_v_scaledPotential = vmin;
	m_previous_scaledPotential = vmin;
	setCurrent(0.0, 0);

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
	}
	else
	{
		func = (m_f) *(( potential * ( potential + m_d ) * ( potential + m_e)) - 0 );
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
		double m_potentialPRIM = 0;
		//m_potentialPRIM = (m_alpha)* (m_v1*(m_v_potential - vzero) / (vmax - vmin) - (1.0 / 3.0) * pow((m_v_potential - vzero) / (vmax - vmin), 3) - m_v_current[0]) + 20*m_stimulation;
		m_potentialPRIM = (m_alpha / m_v1)* (0.6 - pow(m_v_potential - 0.7, 4) * log(m_v_potential + 2.2) - m_v_current[0]) + m_stimulation;
		return m_potentialPRIM;
	}
}
//-------------------------------------------------------------
double vanDerGru::getCurrentPrim(const int& which)
{
	//m_currentPRIM[0] = vanDerGruFunction((m_v_potential - vzero) / (vmax - vmin), m_v_current[0]);
	m_currentPRIM[0] = vanDerGruFunction(m_v_potential, m_v_current[0]);
	return m_currentPRIM[0];
}

void vanDerGru::setERP(double value)
{
	//m_c2 = value;
}
double vanDerGru::getERP()
{
	return 1; //ERP ok 200
}
double vanDerGru::getExcitability()
{
	return 1;
}
void vanDerGru::setExcitability(double value)
{
	//m_ni = value;
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