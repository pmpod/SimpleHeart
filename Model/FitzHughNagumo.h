#pragma once
#include <QObject>
#include "Oscillator.h"
class FitzHughNagumo: public Oscillator
{
	Q_OBJECT

public:
	FitzHughNagumo(void);
	~FitzHughNagumo(void);

	//double fitzHughNagumoFunction(double potential, double current);
	double getCurrentPrim(const int& which);
	double getPotentialPrim();
	double getERP(); ///virtual function for refractory period 
	double getExcitability(); ///virtual function for refractory period setting

	double get_c();
 public slots:
	 void setParameter(double value, OSC_PARAMETER parameter);
	 void setERP(double value); ///virtual function for refractory period setting
	 void setExcitability(double value);

    void setValueBeta(double value);
    void setValueNi(double value);
    void setValueGamma(double value);
	void setDefaultAvParameters();
//signals:

private:

	double m_beta;
	double m_ni;
	double m_gamma;

	double m_c1;
	double m_c2;
	double m_alfa;

};
