#include "v3model.h"

v3model::v3model(void) : Oscillator()
{
	m_type = ATRIAL_V3;
	setParameter(1, V3_CRN);
	m_v_current.clear();
	m_v_current.push_back(0.0);//V, 0
	m_v_current.push_back(0.0);//W, 1
	m_currentPRIM.push_back(0.0);
	m_currentPRIM.push_back(0.0);
	m_currentTime = 0.0;
	m_previousTime = -0.1;

	vzero = 0; //[mV]
	vmax = 1;  //[mV]
	vmin = 0;  //[mV]
	setPotential(0);
	setPreviousPotential(0);
	setCurrent(1, 0);
	setCurrent(1, 1);
}


v3model::~v3model(void)
{
}

//--------------------------------------------------------------
 double v3model::getPotentialPrim()
{
	 m_potentialPRIM = -(get_J_fi() + get_J_so() + get_J_si()) + m_stimulation;
	 return m_potentialPRIM;
}
//--------------------------------------------------------------
 double v3model::getCurrentPrim(const int& which)
 {
	if( which ==0) //V, 0
	{
		m_currentPRIM[0] = ((1 - get_p())*(1 - m_v_current[which]) / get_tau_v_minus()) - (get_p()*m_v_current[which]) / tau_v_plus;
		return m_currentPRIM[0];
			
	}
	else //W, 1
	{
		m_currentPRIM[1] = ((1 - get_p())*(1 - m_v_current[which]) / tau_w_minus) - (get_p()*m_v_current[which]) / tau_w_plus;
		return m_currentPRIM[1];
	}
 }
//--------------------------------------------------------------
double v3model::get_p()
{
	if (this->getPotential() < u_c)
		return 0;
	else
		return 1;
}
//--------------------------------------------------------------
double v3model::get_q()
{
	if (this->getPotential()  < u_v)
		return 0;
	else
		return 1;
}
//--------------------------------------------------------------
double v3model::get_tau_d()
{
	return Cm/g_fi_max;
}
//--------------------------------------------------------------
double v3model::get_J_fi()
{
	return (-m_v_current[0]) * get_p() * (1 - m_v_potential) * (m_v_potential - u_c) / get_tau_d();
}
//--------------------------------------------------------------
double v3model::get_tau_v_minus( )
{
	return (get_q()*qtau_v1_minus)+(1-get_q())*tau_v2_minus;
}
double v3model::get_J_so()
{
	return (((m_v_potential )*(1 - get_p()) / tau_0) + (get_p() / tau_r));
}
double v3model::get_J_si( )
{
	return (-m_v_current[1])*(1 + tanh(k*(m_v_potential - u_cs1))) / (2 * tau_s1);
}
//-------------------------------------------------------------
void v3model::setParameter(double value, OSC_PARAMETER parameter)
{	
	if (parameter == V3_CRN)
	{
		qtau_v1_minus = 40;
		tau_v2_minus = 82.5;
		tau_v_plus = 5.75;
		tau_w_plus = 300;
		tau_w_minus = 100;
		tau_0 = 64.7;
		//tau_r = 222.9;
		tau_r = 122.9;
		u_cs1 = 0.85;
		tau_s1 = 226.9;
		k = 10;
		u_c = 0.16;
		u_v = 0.040;
		Cm = 1.99;
		g_fi_max = Cm / 0.249;
	}
	else if (parameter == V3_modifiedLuoRudy)
	{
		qtau_v1_minus = 18.2;
		tau_v2_minus = 18.2;
		tau_v_plus = 10;
		tau_w_plus = 1020;
		tau_w_minus = 80;
		tau_0 = 12.5;
		tau_r = 130;
		u_cs1 = 0.85;
		tau_s1  = 127;
		k = 10;
		u_c = 0.13;
		u_v = 0.025;
		Cm = 2;
		g_fi_max = 4.4;
	}
	else if (parameter == V3_BeelerReuter)
	{
		g_fi_max = 4;
		tau_r = 33.33;
		tau_s1  = 29;
		tau_0 = 12.5;
		tau_v_plus = 3.33;
		qtau_v1_minus = 1250;
		tau_v2_minus = 19.6;
		tau_w_plus = 870;
		tau_w_minus = 41;
		u_c = 0.13;
		u_v = 0.04;
		u_cs1 = 0.85;
		k = 10;
		Cm = 1;
	}
	else if (parameter == V3_modifiedBeelerReuter)
	{
		g_fi_max = 4;
		tau_r = 50;
		tau_s1  = 45;
		tau_0 = 8.3;
		tau_v_plus = 3.33;
		qtau_v1_minus = 1000;
		tau_v2_minus = 19.2;
		tau_w_plus = 667;
		tau_w_minus = 11;
		u_c = 0.13;
		u_v = 0.055;
		u_cs1 = 0.85;
		k = 10;
		Cm = 1;
	}
	else
	{
		qtau_v1_minus = 40;
		tau_v2_minus = 82.5;
		tau_v_plus = 5.75;
		tau_w_plus = 300;
		tau_w_minus = 100;
		tau_0 = 64.7;
		tau_r = 222.9;
		u_cs1 = 0.85;
		tau_s1 = 226.9;
		k = 10;
		u_c = 0.16;
		u_v = 0.040;
		Cm = 2;
		g_fi_max = Cm / 0.249;
	}
}