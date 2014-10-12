#pragma once
#include <QObject>
#include "Oscillator.h"
class v3model: public Oscillator
{
	Q_OBJECT

public:
	v3model(void);
	~v3model(void);



	double getCurrentPrim(const int& which);	//podaj wynik wzoru na pochodn¹ pr¹du
	double getPotentialPrim();  //podaj wynik wzoru na pochodn¹ potencja³u


	double get_p();
	double get_q();
	double get_tau_d();
	double get_J_fi();
	double get_tau_v_minus();
	double get_J_so();
	double get_J_si();

public slots:
    void setParameter(double value, OSC_PARAMETER parameter);


private:
	float u_c;
	float u_v;
	float Cm;
	float g_fi_max;
	float qtau_v1_minus;
	float tau_v2_minus;
	float tau_v_plus;
	float tau_w_plus;
	float tau_w_minus;
	float tau_0;
	float tau_r;
	float u_cs1;
	float tau_s1;
	float k;
	
};
