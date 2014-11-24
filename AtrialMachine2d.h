#pragma once
#include <QObject>
#include <vector>
#include <cmath>
#include <QtGui/QMessageBox>
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
#include "Control\ProbeElectrode.h"
#include "NumericStrategy\NumericStrategy.h"
#include "Control/epStimulator.h"


class AtrialMachine2d : public QObject
{
	Q_OBJECT
public:
	AtrialMachine2d(atrialParameters* definitions, CardiacMesh *grid);
	~AtrialMachine2d(void);
	void deleteAtrium();
	void init();

	void(AtrialMachine2d::*setStrategy)();


	//-----------------------------------------------

	//-------------------------------------------------------------
	atrialParameters*		m_definitions;//?
	RandomGenerator		generator;//?
	//-------------------------------------------------------------
	std::vector<ProbeElectrode*> probeOscillator;
	CardiacMesh* m_grid;
	NumericStrategy* m_strategy;
	EpStimulator*	stimulator;
	//-------------------------------------------------------------
	double m_globalTime;

	//bool structureUpdated;

private:
	int				_skip;

	//TODO - the same in Mesh??
	//-------------------------------------------------------------
signals:
	void stepFinished();


public slots:
	void setForwardEulerStrategy();
	void setAllexandreStrategy();
	double processStep();

	void startStimulatorProcedure();
	void reset();


	void calculateFullElectrogramMap();
	//----------------simulation settings
	void setSkip(int skip);

	void setUniformDiffusion(double value);
	void setUniformERP(double value);
	//void setS1_Duration(double val);
	//void setS1_Cycle(double val);
	////------------------------------------global nodes settings
	//void setGlobalTime(double t);
	//void setEctoModTime();
	//---------------------------------------------------





};