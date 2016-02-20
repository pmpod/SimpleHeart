#pragma once
#include <QObject>
#include "Oscillator.h"

class vanDerGru : public Oscillator
{
	Q_OBJECT
public:
	vanDerGru(void);
	~vanDerGru(void);
	vanDerGru(CELL_TYPE type);

	void create(CELL_TYPE type);
	
	double vanDerGruFunction(double potential, double current);
	double getCurrentPrim(const int& which);
	double getPotentialPrim();
	double getERP(); ///virtual function for refractory period 
	double getExcitability(); ///virtual function for refractory period setting

	void setDefaultAvParameters();
 public slots:
	void setParameter(double value, OSC_PARAMETER parameter);
	void setERP(double value); ///virtual function for refractory period setting
	void setExcitability(double value);

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
	//ouble m_timeScaler;
};
