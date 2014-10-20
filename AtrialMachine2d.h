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


class AtrialMachine2d: public QObject
{
	Q_OBJECT
public:
	AtrialMachine2d(atrialParameters* definitions, CardiacMesh *grid);
	~AtrialMachine2d(void);
	void init();
	void setAtrium(atrialParameters* definitions, CardiacMesh *grid);

	void deleteAtrium();
//-----------------------------------------------
	//FLAGS
	bool m_hormonalToggle;
//-------------------------------------------------------------
	atrialParameters*		m_definitions;
	RandomGenerator		generator;
	RRcalculator*		RRcalc_1;			// maszyna licz�ca rr-y
	RRcalculator*		RRcalc_2;			// maszyna licz�ca rr-y
	RRcalculator*		RRcalc_3;			// maszyna licz�ca rr-y
	RRcalculator*		RRcalc_4;			// maszyna licz�ca rr-y
	Oscillator*			probeRR_4;
	std::vector<Oscillator*> probeOscillator;

	//std::vector<std::vector<Oscillator*>> m_oscillators;
	std::vector<std::vector<double>> m_diffusionCoefficients;
	std::vector<std::vector<double>> m_anisotrophyCoefficients;
	std::vector<std::pair<Oscillator*,Oscillator*>> m_wallCells;

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

	int m_stimulationID;
//-------------------------------------------------------------
signals:
	void stepFinished();
	void currentHormonalActivity(double,double);
	void emitGlobalTimeOnly(double);
	void currentOscilatorPotential(double, double);
	void currentOscilatorPotential_tab( double* );
	void emitDebug( double);
public slots:
	void processStep();
	void setDiffusionCoefficients();
	void editDiffusionCoefficients();
	void reset();
//----------------simulation settings
	void setSkip(int skip);


//------------------------------------global nodes settings
	void setGlobalTime(double t);
	void setEctoModTime();
//---------------------------------------------------
 //   void setSaGlobalAlpha(double value);
 //   void setSaGlobalF(double value);
 //   void setSaGlobalD(double value);
 //   void setSaGlobalE(double value);
 //   void setSaGlobalV1(double value);
 //   void setSaGlobalV2(double value);
 //   void setSaGlobalTimeScaler(double value);
 //   void setAvGlobalAlpha(double value);
 //   void setAvGlobalF(double value);
 //   void setAvGlobalD(double value);
 //   void setAvGlobalE(double value);
 //   void setAvGlobalV1(double value);
 //   void setAvGlobalV2(double value);
 //   void setAvGlobalTimeScaler(double value);
 //   void setAtriumGlobalGamma(double value);
 //   void setAtriumGlobalBeta(double value);
 //   void setAtriumGlobalNi(double value);
 //   void setAtriumGlobalC1(double value);
 //   void setAtriumGlobalC2(double value);
 //   void setAtriumGlobalAlpha(double value);
 //   void setAtriumGlobalTimeScaler(double value);
	//void setAvBranchGlobalAlpha(double value);
 //   void setAvBranchGlobalF(double value);
 //   void setAvBranchGlobalD(double value);
 //   void setAvBranchGlobalE(double value);
 //   void setAvBranchGlobalV1(double value);
 //   void setAvBranchGlobalV2(double value);
 //   void setAvBranchGlobalTimeScaler(double value);
 //   void setAtriumBranchGlobalGamma(double value);
 //   void setAtriumBranchGlobalBeta(double value);
 //   void setAtriumBranchGlobalNi(double value);
 //   void setAtriumBranchGlobalC1(double value);
 //   void setAtriumBranchGlobalC2(double value);
 //   void setAtriumBranchGlobalAlpha(double value);
 //   void setAtriumBranchGlobalTimeScaler(double value);
	//void setAtriumV3multi(double value);
};
