#pragma once
#include <QObject>
#include <vector>
#include <cmath>
#include <QtGui/QMessageBox>
#include "RRcalculator.h"
#include "Model\Oscillator.h"
#include "Model\vanDerGru.h"
#include "Model\v3model.h"
#include "Model\FitzHughNagumo.h"
#include <fstream>
#include <ctime>
#include <sstream>
#include "RandomGenerator.h"
#include "atrialParameters.h"
#include "Model\CardiacMesh.h"
#include "NumericStrategy\NumericStrategy.h"

class burster
{
public:
	burster();
	double lastSpike;
	int currentPhaseOfPeak;
	double currentPhase;
	double meanPeriod;
	double periodModulation;
	double amp;
	int length;
	double spike(double time,double value);
};


class AtrialMachine2d : public QObject
{
	Q_OBJECT
public:
	AtrialMachine2d(atrialParameters* definitions, CardiacMesh *grid);
	~AtrialMachine2d(void);
	void init();

	void deleteAtrium();
	void setForwardEulerStrategy();
	void setAllexandreStrategy();
	void(AtrialMachine2d::*setStrategy)();

	//-----------------------------------------------
	//FLAGS
	bool m_hormonalToggle;
	//-------------------------------------------------------------
	atrialParameters*		m_definitions;
	RandomGenerator		generator;
	RRcalculator*		RRcalc_1;			// maszyna licz¹ca rr-y
	RRcalculator*		RRcalc_2;			// maszyna licz¹ca rr-y
	RRcalculator*		RRcalc_3;			// maszyna licz¹ca rr-y
	RRcalculator*		RRcalc_4;			// maszyna licz¹ca rr-y
	Oscillator*			probeRR_4;
	std::vector<Oscillator*> probeOscillator;

	//std::vector<std::vector<Oscillator*>> m_oscillators;
	std::vector<std::vector<double>> m_diffusionCoefficients;
	std::vector<std::vector<double>> m_anisotrophyCoefficients;
	std::vector<std::pair<Oscillator*, Oscillator*>> m_wallCells;

	CardiacMesh* m_grid;
	NumericStrategy* m_strategy;
	//-------------------------------------------------------------
	double m_globalTime;
	int	   m_skip;
	double m_mainTimestep;
	double nextTimestep;
	double nextTime;
	double nextPotential;
	double nextCurrent[3];

	bool structureUpdated;
	//-------------------------------------------------------------
signals:
	void stepFinished();
	void currentHormonalActivity(double, double);
	void currentOscilatorPotential(double, double);
	void currentOscilatorPotential_tab(double*);
	void emitDebug(double);
	public slots:
	double processStep();
	void reset();
	void calculateFullElectrogramMap();
	//----------------simulation settings
	void setSkip(int skip);

	//------------------------------------global nodes settings
	void setGlobalTime(double t);
	void setEctoModTime();
	//---------------------------------------------------
};