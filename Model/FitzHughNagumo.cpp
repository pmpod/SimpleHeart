#include "FitzHughNagumo.h"
#include <cmath>
FitzHughNagumo::FitzHughNagumo(void):
	Oscillator()
{
	m_beta = 1.1;//0.7;
	m_ni = 0.009; //0.01//0,3
	m_gamma = 0.4;//0.5;
	m_c1 = 0.24;//;2.2;
	m_c2 =  0.9;//2.0;
	m_alfa = 0.4;//0.05;
	m_v_current.clear();//V, 0
	m_v_current.push_back(0.0);//V, 0
	//m_timeScaler = 1.0;
	//xmax = 1.8; 
	//xmin = -2.03;
	//type3d =0;
}
//-----------------------------------------------------
double FitzHughNagumo::getCurrentPrim(int which)
{
	double func = 0;

//FHN zwyk³e moje
	//func = ( m_ni ) * ( m_v_potential + m_beta - m_gamma * current );
//FHN Rogers-McCulloch + m_beta
	func = (m_ni)* (m_v_potential - m_gamma * m_c2)*(m_v_current[0]);
//FHN Inada Boyett
	return func;
}
//-----------------------------------------------------
double FitzHughNagumo::getPotentialPrim()
{
	double func = 0;

//FHN zwyk³e moje
	//func = (m_alfa) * ( potential - (m_c1) * pow( potential, 3) - (m_c2)*m_v_current ) + m_drive ;
//FHN Rogers-McCulloch
	func = (m_c1)* m_v_potential * (m_v_potential - m_alfa) * (1.5 - m_v_potential) - (m_c2)*(m_v_current[0] + 1.0) * (m_v_potential + 1.0);
//FHN Inada Boyett

	return func;
}
//-----------------------------------------------------
FitzHughNagumo::~FitzHughNagumo(void)
{
}
void FitzHughNagumo::setDefaultAvParameters()
{

}
//----------------------------------------------------
void FitzHughNagumo::setParameter(double value, OSC_PARAMETER parameter)
{
	if(parameter == FHN_BETA)
	{	
		m_beta = value;
	}
	else if(parameter == FHN_GAMMA)
	{	
		m_gamma = value;
	}
	else if(parameter == FHN_NI)
	{	
		m_ni = value;
	}
	else if(parameter == FHN_C1)
	{	
		m_c1 = value;
	}
	else if(parameter == FHN_C2)
	{	
		m_c2 = value;
	}
	else if(parameter == FHN_ALPHA)
	{	
		m_alfa = value;
	}
}
//-----------------------------------------------------
void FitzHughNagumo::setValueBeta(double value) 
{ 
	//setParameter(&m_beta,value);
}
//-----------------------------------------------------
void FitzHughNagumo::setValueNi(double value) 
{ 
	//setParameter(&m_ni,value);
}
//-----------------------------------------------------
void FitzHughNagumo::setValueGamma(double value) 
{ 
	//setParameter(&m_gamma,value);
}