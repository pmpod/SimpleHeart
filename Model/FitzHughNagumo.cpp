#include "FitzHughNagumo.h"
#include <cmath>
FitzHughNagumo::FitzHughNagumo(void): Oscillator()
{
	m_type = ATRIAL_FHN;

	//FHN_DEFAULT
	//m_beta = 0.4;//0.7;
	//m_ni = 0.02; //0.01//0,3
	//m_gamma = 1.3;//0.5;
	//m_c1 = 0.33;//;2.2;
	//m_c2 = 1;//2.0;
	//m_alfa = 0.7;//0.05;


	//////FHNR_DEFAULT
	//m_beta = 1.1;//0.7;
	//m_ni = 0.09; //0.01//0,3
	//m_gamma = 0.4;//0.5;
	//m_c1 = 0.24;//;2.2;
	//m_c2 =  0.9;//2.0;
	//m_alfa = 0.4;//0.05;

	//FHN_MUSCLE_LI
	m_beta = 1.1;//0.7;

	m_alfa = 0.1;//0.05;
	m_c1 = 12.7;//;2.2;
	m_c2 = 2.84;//2.0;
	m_ni = 0.01; //b
	m_gamma = 2.475;   //d



	m_v_current.clear();
	m_v_current.push_back(0.0);//V, 0
	m_currentPRIM.clear();
	m_currentPRIM.push_back(0.0);
	m_currentTime = 0.0;
	m_previousTime = -0.1;

	vzero = 0; //[mV]
	vmax = 1.0;  //[mV]
	vmin = 0;  //[mV]

	m_v_potential = 0;
	m_previous_potential = 0;
	m_v_scaledPotential = vmin;
	m_previous_scaledPotential = vmin;
	setCurrent(0.0, 0);
	//m_timeScaler = 1.0;
	//xmax = 1.8; 
	//xmin = -2.03;
	//type3d =0;
}
//-----------------------------------------------------
double FitzHughNagumo::getCurrentPrim(const int& which)
{
//FHN zwyk³e moje
	//m_currentPRIM[0] = (m_ni)* (m_v_potential + m_beta - m_gamma * m_v_current[0]);
//FHN Rogers-McCulloch + m_beta
	//m_currentPRIM[0] = (m_ni)* (m_v_potential - m_gamma * m_c2)*(m_v_current[0]);
//FHN Inada Boyett
	m_currentPRIM[0] = (m_ni)* (m_v_potential - m_gamma * m_v_current[0]);
	return m_currentPRIM[0];
}
//-----------------------------------------------------
double FitzHughNagumo::getPotentialPrim()
{
	//FHN zwyk³e moje
	//m_potentialPRIM = (m_alfa)* (m_v_potential - (m_c1)* pow(m_v_potential, 3) - (m_c2)*m_v_current[0]) + m_stimulation;
	//FHN Rogers-McCulloch
	//m_potentialPRIM = (m_c1)* m_v_potential * (m_v_potential - m_alfa) * (1.5 - m_v_potential) - (m_c2)*(m_v_current[0] + 1.0) * (m_v_potential + 1.0) +  m_stimulation;
	//m_potentialPRIM = (m_c1)* m_v_potential * (m_v_potential - m_alfa) * (1.0 - m_v_potential) - (m_c2)*(m_v_current[0] ) * (m_v_potential) + 10 * m_stimulation;
	//FHN Rogers-McCulloch-Li
	m_potentialPRIM = get_c()* (m_v_potential * (m_v_potential - m_alfa) * (1.0 - m_v_potential) - m_v_potential*m_v_current[0]) + m_stimulation;

	return m_potentialPRIM;
}
//--------------------------------------------------------------
double FitzHughNagumo::get_c()
{
	if (this->m_potentialPRIM >= 0)
		return m_c1;
	else
		return m_c2;
}
void FitzHughNagumo::setERP(double value)
{
	m_c2 = value;
}
double FitzHughNagumo::getERP()
{
	return m_c2; //ERP ok 200
}
double FitzHughNagumo::getExcitability()
{
	return m_ni;
}
void FitzHughNagumo::setExcitability(double value)
{
	m_ni = value;
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