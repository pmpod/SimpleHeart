#pragma once
#include "heartDefines.h"
#include <vector>
#include <utility>
#include <QObject>
#include "atrialParameters.h"

using namespace std;

class RRcalculator: public QObject
{
	Q_OBJECT

	atrialParameters* m_definitions;
	int rangeOffocus;
	double lastMaxRR;
	double currentMaxRR;
	int maxRRindex;

	//int maxRrTableSize;   //TODO not used yet

public:
	void setRangeOfFocus(int range);
	std::vector<double> intervalsRR;
	std::vector<double> intervalsPotential;
	std::vector<double> intervalsSaved;


	RRcalculator(atrialParameters* definitions);
	~RRcalculator(void);



	std::vector<double> m_histogram;
	std::vector<double> m_histogram_0;
	std::vector<double> m_histogram_1t;
	std::vector<double> m_histogram_2t;

	double m_patEn;
	double m_varEn;


public slots:
	void reset();
	void processNewTime(double time, double potential);
	void calculatePatEn(double rr);
	void calculateVarEn(double rr);
signals:
	void nextRR( double rr );
	void curIndex(int i);
	void nextPatEn( double value);
	void nextVarEn( double value);
};
