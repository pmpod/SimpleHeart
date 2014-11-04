#include "AtrialMachine2d.h"
#include "NumericStrategy\ForwardEulerStrategy.h"
#include "NumericStrategy\AllexandreStrategy.h"

#define CYLINDER 0
#define ELECTROGRAMMM 1
burster::burster()
{
	lastSpike = 0;
	currentPhaseOfPeak = 0;
	currentPhase = 0;
	meanPeriod = 70;
	periodModulation = 0;
	amp = -0.1;
	length = 1;
}
double burster::spike(double time, double value)
{
	double retValue;
	currentPhase = time - lastSpike;
	if(currentPhaseOfPeak == 0 )
	{
		periodModulation = value;
		if(meanPeriod + periodModulation<=length+1)
		{
			periodModulation=length+1-meanPeriod;
		}
		lastSpike = time;
		retValue = amp;
		currentPhaseOfPeak++;
	}
	else if(currentPhaseOfPeak < length)
	{
		retValue = amp;
		currentPhaseOfPeak++;
	}
	else
	{
		retValue = 0;
	}

	if(currentPhase>= meanPeriod + periodModulation)
	{
		currentPhaseOfPeak=0;
		lastSpike = time;
	}
	return retValue;
}
AtrialMachine2d::AtrialMachine2d(atrialParameters* definitions, CardiacMesh *grid)
{
	m_strategy = nullptr;
	m_definitions = definitions;
	init();

	m_grid = grid;
	m_grid->m_ectopicAmplitude = definitions->m_ectopicAmplitude;
	
	probeOscillator.push_back(m_grid->m_mesh[0]);
	probeOscillator.push_back(m_grid->m_mesh[2000]);
	probeOscillator.push_back(m_grid->m_mesh[5000]);

	setForwardEulerStrategy();
}
//-------------------------------------------------------------------------
AtrialMachine2d::~AtrialMachine2d(void)
{

}
//-------------------------------------------------------------------------
void AtrialMachine2d::setForwardEulerStrategy()
{
	if (m_strategy != nullptr)
		delete(m_strategy);
	m_strategy = new ForwardEulerStrategy(m_grid);
	setStrategy = &AtrialMachine2d::setForwardEulerStrategy;
}
//-------------------------------------------------------------------------
void AtrialMachine2d::setAllexandreStrategy()
{
	if (m_strategy != nullptr)
		delete(m_strategy);
	m_strategy = new AllexandreStrategy(m_grid);
	setStrategy = &AtrialMachine2d::setAllexandreStrategy;
}
//-------------------------------------------------------------------------
void AtrialMachine2d::init()
{
	RRcalc_1 = new RRcalculator(m_definitions);
	RRcalc_2 = new RRcalculator(m_definitions);
	RRcalc_3 = new RRcalculator(m_definitions);
	RRcalc_4 = new RRcalculator(m_definitions);

	 m_globalTime = 0;
	 m_mainTimestep = m_definitions->m_mainTimestep;
	 m_skip = m_definitions->m_mainSkip;
}
//------------------------------------------------------------------------------------------
void AtrialMachine2d::reset()
{
	init();
	m_strategy->reset();
}
//------------------------------------------------------------------------------------------
double AtrialMachine2d::processStep() 
{
	if (m_grid->structureUpdated)
	{
		m_strategy->synchronise();
		(this->*setStrategy)();
		m_grid->structureUpdated = false;
	}
	

	for (int kk = 0; kk <= 30000; ++kk)//m_skip
	{ 
		m_globalTime = m_strategy->nextStep();
	}

	
	return m_globalTime;


}
//------------------------------------------------------------------------------------------
void AtrialMachine2d::setEctoModTime()
{
	m_definitions->setEctoModTime(m_globalTime);
}
void AtrialMachine2d::calculateFullElectrogramMap()
{
	m_strategy->synchronise();
	double minP = 0;
	double maxP = 0;

	for (int j = 0; j < m_grid->m_mesh.size(); ++j)
	{
		m_grid->calculateElectrogram(m_grid->m_mesh[j]);
		if (m_grid->m_mesh[j]->m_v_electrogram < minP)
			minP = m_grid->m_mesh[j]->m_v_electrogram;
		if (m_grid->m_mesh[j]->m_v_electrogram > maxP)
			maxP = m_grid->m_mesh[j]->m_v_electrogram;
	}
	m_grid->m_minElectrogram = minP;
	m_grid->m_maxElectrogram = maxP;
}
//--------------------------------------------------------------------
void AtrialMachine2d::setSkip(int skip) { 	m_skip = skip; }

void AtrialMachine2d::setGlobalTime(double t)
{
	m_globalTime = t;
}