#pragma once
//#include <qwt_data.h>
#include<vector>
#include<deque>
#include <QObject>
#include "Model\Oscillator.h"

class ProbeElectrode : public QObject
{
	Q_OBJECT
    // A container class for growing data
public:

	ProbeElectrode();
	~ProbeElectrode();
	ProbeElectrode(Oscillator*);
	void setOscillator(Oscillator*);
	double getPositionX();
	double getPositionY();
	double getPositionZ(); 
	double getOscillatorID();
	double lastRR();
	double currentMaxRR();
	void setDisplayValue(int value);

private:

	void setRangeOfFocus(int range);
	std::deque<double> intervalsRR;
	std::deque<double> intervalsPotential;
	std::deque<double> intervalsSaved;


	Oscillator* _osc;
	int rangeOffocus;
	double lastMaxRR;
	double _currentMaxRR;
	int maxRRindex;
	double refractoryPeriod;
	double potentialTreshold;
	double _lastRR;
	int displayGraphValue;


public slots:
	void reset();
	void processNewTime(double time);
signals:
	void nextRR(double rr);
	void curIndex(int i);
	void newElectrogramAndTime(double, double);
};
