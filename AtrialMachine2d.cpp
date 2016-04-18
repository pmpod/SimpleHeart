#include "AtrialMachine2d.h"
#include "NumericStrategy\ForwardEulerStrategy.h"
#include "NumericStrategy\AllexandreStrategy.h"

#define ELECTROGRAMMM 1

AtrialMachine2d::AtrialMachine2d(atrialParameters* definitions, CardiacMesh *grid)
{
	m_strategy = nullptr;
	m_definitions = definitions;
	stimulator = EpStimulator::Instance();
	init();

	m_grid = grid;

	stimulator->setProbeElectrode(m_grid, 0, 4*100+25);// 129);
	stimulator->setProbeElectrode(m_grid, 1, 246*100-25);// 16257 - 128);
	stimulator->setProbeElectrode(m_grid, 2, 10+75*100);// 5000);

	setForwardEulerStrategy();
	//setAllexandreStrategy();
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
	 m_globalTime = 0;
	 _skip = m_definitions->m_mainSkip;
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

	//m_grid->minElectrogram = m_grid->minPotential;
	//m_grid->maxElectrogram = m_grid->maxPotential;
	//else {
	//	stimulator.stop(m_grid);
	//}

	//if (m_definitions->m_ectopicActivity && stimulationBegun == false)
	//{
	//	startStimulation(m_grid->m_mesh[m_grid->m_stimulationID], m_grid->m_stimulationID, 2, 0.1);
	//}

	//else if (!m_definitions->m_ectopicActivity && stimulationBegun == true)
	//{
	//	stopStimulation();
	//}
	

	for (int kk = 0; kk < 20; ++kk)//m_skip
	{ 
		m_globalTime = m_strategy->nextStep();
	}

	m_grid->calculateElectrogramGrid();
	m_grid->calculateElectrogram(m_grid->m_mesh[stimulator->probeElectrode(0)->getOscillatorID()]);
	m_grid->calculateElectrogram(m_grid->m_mesh[stimulator->probeElectrode(1)->getOscillatorID()]);
	m_grid->calculateElectrogram(m_grid->m_mesh[stimulator->probeElectrode(2)->getOscillatorID()]);

	stimulator->run(m_grid, m_globalTime);

	return m_globalTime;


}
//------------------------------------------------------------------------------------------
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
	m_grid->minElectrogram = minP;
	m_grid->maxElectrogram = maxP;
}
//--------------------------------------------------------------------
void AtrialMachine2d::setSkip(int skip) { 	_skip = skip; }

void AtrialMachine2d::setUniformDiffusion(double value)
{
	for (int j = 0; j < m_grid->m_mesh.size(); ++j)
	{
		m_grid->m_mesh[j]->setSigma(value, value, 0);
	}
}
void AtrialMachine2d::setUniformERP(double value)
{
	for (int j = 0; j < m_grid->m_mesh.size(); ++j)
	{
		m_grid->m_mesh[j]->setERP(value);
	}
}

void AtrialMachine2d::startStimulatorProcedure()
{
	stimulator->start(m_grid);
}

void AtrialMachine2d::resetEpStimulator()
{
	stimulator->stop(m_grid);
}
