#pragma once
#include <QObject>
#include "Oscillator.h"

class vanDerGru : public Oscillator
{
	Q_OBJECT
public:
	vanDerGru(void);
	~vanDerGru(void);
	
	double vanDerGruFunction(double potential, double current);
	double getCurrentPrim(int which);
	double getPotentialPrim();

	void setDefaultAvParameters();
 public slots:
    void setParameter(double value, OSC_PARAMETER parameter);
    void setValueAlpha(double value);
    void setValueF(double value);
    void setValueD(double value);
    void setValueE(double value);
    void setValueV1(double value);
    void setValueV2(double value);
    //void setValueTimeScaler(double value);

 signals:
    void valueAlphaChanged(double newValue);

private:

	double m_alpha;
	double m_f;
	double m_d;
	double m_e;
	double m_v1;
	double m_v2;
	int m_vdPtype;
	double vzero;//-0.5;
	double vmax;////1.3;
	double vmin;//-0.5;
	//ouble m_timeScaler;
};
