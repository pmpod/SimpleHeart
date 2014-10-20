#include "AtrialMachine2d.h"
#include "NumericStrategy\ForwardEulerStrategy.h"
#include "NumericStrategy\AllexandreStrategy.h"
#include <boost\foreach.hpp>

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
	m_definitions = definitions;
	init();

	m_grid = grid;
	m_grid->m_ectopicAmplitude = definitions->m_ectopicAmplitude;

	setAtrium(definitions,grid);
	setDiffusionCoefficients();
	setWallCells();

	defibrillate();

	probeOscillator.push_back(m_grid->m_mesh[0]);
	probeOscillator.push_back(m_grid->m_mesh[2000]);
	probeOscillator.push_back(m_grid->m_mesh[5000]);

	//m_strategy = new ForwardEulerStrategy(m_grid);
	m_strategy = new AllexandreStrategy(m_grid);
}
//-------------------------------------------------------------------------
AtrialMachine2d::~AtrialMachine2d(void)
{

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

	 //currentAlgorithm = SW_EULER;
	 m_skip = m_definitions->m_mainSkip;

	 m_whichFlag = 1;
 	 m_initial_v = 0;
	 m_initial_c = 1;
	 m_initial_c_2 = 1;

}
//-----------------------------
void AtrialMachine2d::setAtrium(atrialParameters* definitions, CardiacMesh *grid)
{
	deleteAtrium();
	//m_definitions = definitions;
	m_grid = grid;
	double t1 = 0.3;
	double t2 = 0.5;
	for (int j = 0; j < m_grid->getSize(); ++j)
	{
		vector<double> temp2;
		vector<double> temp3;
		m_diffusionCoefficients.push_back(temp2);
		m_anisotrophyCoefficients.push_back(temp3);
		for (int i = 0; i < m_grid->getSize(); ++i)
		{
					m_diffusionCoefficients[j].push_back( t1 );
					m_anisotrophyCoefficients[j].push_back(t2);
		}
	}
	//for (int j = 0; j < m_grid->getSizeY(); ++j)
	//{
	//	vector<Oscillator*> temp;
	//	vector<double> temp2;
	//	vector<double> temp3;
	//	m_oscillators.push_back( temp );
	//	m_diffusionCoefficients.push_back(temp2);
	//	m_anisotrophyCoefficients.push_back(temp3);

	//	for (int i = 0; i <  m_grid->getSizeX(); ++i)
	//	{
	//		double t1 = 0.3;
	//		double t2 = 0.5;
	//		m_diffusionCoefficients[j].push_back( t1 );
	//		m_anisotrophyCoefficients[j].push_back(t2);
	//		if(m_grid->m_net[j][i]->m_type == SA_NODE)
	//		{
	//			vanDerGru * vDg = new vanDerGru();
	//			m_oscillators[j].push_back( vDg );
	//			m_oscillators[j].back()->setType(SA_NODE);
	//		}
	//		else if (m_grid->m_net[j][i]->m_type == ATRIAL_TISSUE)
	//		{
	//			FitzHughNagumo * fHn = new FitzHughNagumo();
	//			m_oscillators[j].push_back( fHn );
	//			m_oscillators[j].back()->setType(ATRIAL_TISSUE);
	//		}
	//		else if (m_grid->m_net[j][i]->m_type == ATRIAL_V3)
	//		{
	//			v3model * v3m = new v3model();
	//			m_oscillators[j].push_back( v3m );
	//			m_oscillators[j].back()->setType(ATRIAL_V3);
	//		}
	//		else if(m_grid->m_net[j][i]->m_type == AV_NODE)
	//		{
	//			vanDerGru * vDg = new vanDerGru();
	//			m_oscillators[j].push_back( vDg );
	//			m_oscillators[j].back()->setType(AV_NODE);
	//		}
	//		else if(m_grid->m_net[j][i]->m_type == AV_NODE_BRANCH)
	//		{
	//			vanDerGru * vDg = new vanDerGru();
	//			m_oscillators[j].push_back( vDg );
	//			m_oscillators[j].back()->setType(AV_NODE_BRANCH);
	//		}
	//		else if(m_grid->m_net[j][i]->m_type == ATRIAL_BRANCH)
	//		{
	//			FitzHughNagumo * fHn = new FitzHughNagumo();
	//			m_oscillators[j].push_back( fHn );
	//			m_oscillators[j].back()->setType(ATRIAL_BRANCH);
	//		}
	//		else if(m_grid->m_net[j][i]->m_type == PURKINIE_BUNDLE)
	//		{
	//			FitzHughNagumo * fHn = new FitzHughNagumo();
	//			m_oscillators[j].push_back( fHn );
	//			m_oscillators[j].back()->setType(PURKINIE_BUNDLE);
	//		}
	//		else if(m_grid->m_net[j][i]->m_type == SOLID_WALL)
	//		{
	//			Oscillator * osc = new Oscillator();
	//			m_oscillators[j].push_back( osc );
	//			m_oscillators[j].back()->setType(SOLID_WALL);
	//		}
	//		else
	//		{
	//			Oscillator * osc = new Oscillator();
	//			m_oscillators[j].push_back( osc );
	//			m_oscillators[j].back()->setType(NONE);
	//		}
	//		m_oscillators[j].back()->setPotential(m_initial_v);
	//		if (m_oscillators[j][i]->getCellType() == ATRIAL_V3)
	//		{
	//			m_oscillators[j].back()->setCurrent(m_initial_c, 0);
	//			m_oscillators[j].back()->setCurrent(m_initial_c_2, 1);
	//		}
	//		m_oscillators[j].back()->setPreviousPotential( m_initial_v );
	//		m_oscillators[j].back()->setPositionX(static_cast<double>(i) * m_grid->getDeltaX());
	//		m_oscillators[j].back()->setPositionY(static_cast<double>(j)* m_grid->getDeltaY());
	//		m_oscillators[j].back()->setPositionZ(0);
	//	}
	//}
}
void AtrialMachine2d::resetAnisotrophyCoefficients()
{
	//for(int i = 0; i<m_grid->getSize(); ++i) 
	//{
	//	for (int j = 0; j<m_grid->getSize() ; ++j)	
	//	{
	//			m_grid->m_net[i][j]->m_anisotrophy_L=0.5;
	//			m_grid->m_net[i][j]->m_anisotrophy_R=0.5;
	//	}
	//}
}
//--------------------------------------------------
void AtrialMachine2d::setAtrialStructure()
{
	//for (unsigned int j = 0; j < m_grid->m_net.size(); ++j)
	//{
	//	for (unsigned int i = 0; i <  m_grid->m_net[j].size(); ++i)
	//	{
	//		if(m_grid->m_net[j][i]->m_type == SA_NODE)
	//		{
	//			vanDerGru * vDg = new vanDerGru();
	//			m_oscillators[j][i] = NULL;
	//			m_oscillators[j][i] = vDg;
	//			m_oscillators[j][i]->setType(SA_NODE);
	//			m_oscillators[j][i]->setCurrentTime(m_globalTime);
	//		}
	//		else if (m_grid->m_net[j][i]->m_type == ATRIAL_TISSUE)
	//		{
	//			FitzHughNagumo * fHn = new FitzHughNagumo();
	//			m_oscillators[j][i] = NULL;
	//			m_oscillators[j][i] = fHn;
	//			m_oscillators[j][i]->setType(ATRIAL_TISSUE);
	//			m_oscillators[j][i]->setCurrentTime(m_globalTime);
	//		}
	//		else if (m_grid->m_net[j][i]->m_type == ATRIAL_V3)
	//		{
	//			v3model * v3m = new v3model();
	//			m_oscillators[j][i] = NULL;
	//			m_oscillators[j][i] = v3m;
	//			m_oscillators[j][i]->setType(ATRIAL_V3);
	//			m_oscillators[j][i]->setCurrentTime(m_globalTime);
	//		}
	//		else if(m_grid->m_net[j][i]->m_type == AV_NODE)
	//		{
	//			vanDerGru * vDg = new vanDerGru();	
	//			m_oscillators[j][i] = NULL;
	//			m_oscillators[j][i] = vDg;
	//			m_oscillators[j][i]->setType(AV_NODE);
	//			m_oscillators[j][i]->setCurrentTime(m_globalTime);
	//		}
	//		else if(m_grid->m_net[j][i]->m_type == AV_NODE_BRANCH)
	//		{
	//			vanDerGru * vDg = new vanDerGru();
	//			m_oscillators[j][i] = NULL;				
	//			m_oscillators[j][i] = vDg;
	//			m_oscillators[j][i]->setType(AV_NODE_BRANCH);
	//			m_oscillators[j][i]->setCurrentTime(m_globalTime);
	//		
	//		}
	//		else if(m_grid->m_net[j][i]->m_type == ATRIAL_BRANCH)
	//		{
	//			FitzHughNagumo * fHn = new FitzHughNagumo();	
	//			m_oscillators[j][i] = NULL;
	//			m_oscillators[j][i] = fHn;
	//			m_oscillators[j][i]->setType(ATRIAL_BRANCH);
	//			m_oscillators[j][i]->setCurrentTime(m_globalTime);
	//		}
	//		else if(m_grid->m_net[j][i]->m_type == PURKINIE_BUNDLE)
	//		{
	//			FitzHughNagumo * fHn = new FitzHughNagumo();	
	//			m_oscillators[j][i] = NULL;
	//			m_oscillators[j][i] = fHn;
	//			m_oscillators[j][i]->setType(PURKINIE_BUNDLE);
	//			m_oscillators[j][i]->setCurrentTime(m_globalTime);
	//		}
	//		else if(m_grid->m_net[j][i]->m_type == SOLID_WALL)
	//		{
	//			Oscillator * osc = new Oscillator();	
	//			m_oscillators[j][i] = NULL;
	//			m_oscillators[j][i] = osc;
	//			m_oscillators[j][i]->setType(SOLID_WALL);
	//			m_oscillators[j][i]->setCurrentTime(m_globalTime);
	//		}
	//		else
	//		{
	//			Oscillator * osc = new Oscillator();
	//			m_oscillators[j][i] = NULL;
	//			m_oscillators[j][i] = osc;
	//			m_oscillators[j][i]->setType(NONE);
	//			m_oscillators[j][i]->setCurrentTime(m_globalTime);
	//		}
	//		m_oscillators[j][i]->setPotential(m_initial_v);
	//		if (m_oscillators[j][i]->getCellType() == ATRIAL_V3)
	//		{
	//			m_oscillators[j][i]->setCurrent(m_initial_c, 0);
	//			m_oscillators[j][i]->setCurrent(m_initial_c_2, 1);
	//		}
	//		m_oscillators[j][i]->setPreviousPotential(m_initial_v);
	//		m_oscillators[j][i]->setPositionX(static_cast<double>(i)* m_grid->getDeltaX());
	//		m_oscillators[j][i]->setPositionY(static_cast<double>(j)* m_grid->getDeltaY());
	//		m_oscillators[j][i]->setPositionZ(0);
	//	}
	//}
	//setDiffusionCoefficients();
	//setWallCells();
}
//-----------
void AtrialMachine2d::setWallCells()
{
	m_wallCells.clear();
	//for (unsigned int j = 0; j < m_oscillators.size(); ++j)
	//{
	//	if(!CYLINDER) m_wallCells.push_back( make_pair( m_oscillators[j][0], m_oscillators[j][1]));
	//	if(!CYLINDER) m_wallCells.push_back( make_pair( m_oscillators[j][m_oscillators[j].size()-1], m_oscillators[j][m_oscillators[j].size()-2]));
	//	m_wallCells.push_back( make_pair( m_oscillators[0][j], m_oscillators[1][j]));
	//	m_wallCells.push_back( make_pair( m_oscillators[m_oscillators[j].size()-1][j], m_oscillators[m_oscillators[j].size()-2][j]));		
	//}
	//for (unsigned int j = 1; j < m_grid->m_net.size()-1; ++j)
	//{
	//	for (unsigned int i = 1; i <  m_grid->m_net[j].size()-1; ++i)
	//	{
	//		if( m_grid->m_net[j][i]->m_type == SOLID_WALL )
	//		{
	//			if( m_grid->m_net[j][i-1]->m_type != SOLID_WALL &&
	//				m_grid->m_net[j][i-1]->m_type != NONE )
	//			{
	//				m_wallCells.push_back( make_pair( m_oscillators[j][i], m_oscillators[j][i-1]));
	//			}
	//			else if( m_grid->m_net[j][i+1]->m_type != SOLID_WALL &&
	//					 m_grid->m_net[j][i+1]->m_type != NONE )
	//			{
	//				m_wallCells.push_back( make_pair( m_oscillators[j][i], m_oscillators[j][i+1]));
	//			}
	//			else if( m_grid->m_net[j-1][i]->m_type != SOLID_WALL &&
	//					 m_grid->m_net[j-1][i]->m_type != NONE )
	//			{
	//				m_wallCells.push_back( make_pair( m_oscillators[j][i], m_oscillators[j-1][i]));
	//			}
	//			else if( m_grid->m_net[j+1][i]->m_type != SOLID_WALL &&
	//					 m_grid->m_net[j+1][i]->m_type != NONE )
	//			{
	//				m_wallCells.push_back( make_pair( m_oscillators[j][i], m_oscillators[j+1][i]));
	//			}
	//		}
	//	}
	//}
}
//-----------
void AtrialMachine2d::deleteAtrium()
{
	  //  for( std::vector<std::vector<Oscillator*>>::iterator it = m_oscillators.begin(); it != m_oscillators.end(); ++it)
   //      for( std::vector<Oscillator*>::iterator it2 = it->begin(); it2 != it->end(); ++it2)
			//delete *it2;  
}
//-----------
void AtrialMachine2d::reset(atrialParameters* definitions, CardiacMesh *grid)
{

	init();
	//setAtrium(definitions,grid);
	//setDiffusionCoefficients();

	defibrillate();
}
//----------------
void AtrialMachine2d::defibrillate()
{
	m_globalTime = 0;
	int gridSize = m_grid->m_mesh.size();

	for (int j = 0; j < gridSize; ++j)
	{
		Oscillator *osc = m_grid->m_mesh[j];
		osc->setPotential(m_initial_v);
		osc->setPreviousPotential(m_initial_v);
		if (osc->getCellType() == ATRIAL_V3)
		{
			osc->setCurrent(m_initial_c, 0);
			osc->setCurrent(m_initial_c_2, 1);
		}
		osc->setCurrentTime(0.0);
		osc->setCurrentTime(0.0);
	}
}
//----------------
void AtrialMachine2d::setDiffusionCoefficients()
{
	int gridSize = m_grid->m_mesh.size();
	int gridWidth = m_grid->getSize();

	for (int j = 0; j < gridSize; ++j)
	{
		Oscillator *osc = m_grid->m_mesh[j];
		int ii = static_cast<double> (j % gridWidth);
		int jj = floor(static_cast<double>(j) / static_cast<double>(gridWidth));

		osc->setSigma(m_diffusionCoefficients[ii][jj] * m_anisotrophyCoefficients[ii][jj],
			m_diffusionCoefficients[ii][jj] * (1 - m_anisotrophyCoefficients[ii][jj]),
			0);
	}
	//if(!CYLINDER)
	//{
	//	for (int j = 1; j < m_oscillators.size()-1; ++j)
	//	{
	//		for (int i = 1; i <  m_oscillators[j].size()-1; ++i)
	//		{
	//			m_oscillators[j][i]->addNeighbour(m_oscillators[j][i + 1]);
	//			// m_diffusionCoefficients[j][i + 1] * m_anisotrophyCoefficients[j][i + 1]);
	//			m_oscillators[j][i]->addNeighbour(m_oscillators[j][i - 1]);
	//			//, m_diffusionCoefficients[j][i - 1] * m_anisotrophyCoefficients[j][i - 1]);
	//			m_oscillators[j][i]->addNeighbour(m_oscillators[j + 1][i]);
	//			//, m_diffusionCoefficients[j + 1][i] * (1 - m_anisotrophyCoefficients[j + 1][i]));
	//			m_oscillators[j][i]->addNeighbour(m_oscillators[j - 1][i]);
	//			//, m_diffusionCoefficients[j - 1][i] * (1 - m_anisotrophyCoefficients[j - 1][i]));
	//		}
	//	}
	//}

	//if(CYLINDER)
	//{
	//	for (int j = 1; j < m_oscillators.size()-1; ++j)
	//	{
	//		for (int i = 1; i <  m_oscillators[j].size()-1; ++i)
	//		{
	//			m_oscillators[j][i]->addNeighbour(m_oscillators[j][i + 1]);
	//			//,m_diffusionCoefficients[j][i+1]*m_anisotrophyCoefficients[j][i+1]);
	//			m_oscillators[j][i]->addNeighbour(m_oscillators[j][i - 1]);
	//			//,m_diffusionCoefficients[j][i-1]*m_anisotrophyCoefficients[j][i-1]);
	//			m_oscillators[j][i]->addNeighbour(m_oscillators[j + 1][i]);
	//			//,m_diffusionCoefficients[j+1][i]*(1-m_anisotrophyCoefficients[j+1][i]));
	//			m_oscillators[j][i]->addNeighbour(m_oscillators[j - 1][i]);
	//			//,m_diffusionCoefficients[j-1][i]*(1-m_anisotrophyCoefficients[j-1][i]));
	//		}
	//	}
	//	for (int j = 0; j < m_oscillators.size(); ++j)
	//	{
	//		m_oscillators[j][0]->addNeighbour(m_oscillators[j][1]);// , m_diffusionCoefficients[j][1] * m_anisotrophyCoefficients[j][1]);
	//		if (j != m_oscillators.size() - 1)	m_oscillators[j][0]->addNeighbour(m_oscillators[j + 1][0]);//,m_diffusionCoefficients[j+1][0]*(1-m_anisotrophyCoefficients[j+1][0]));
	//		if (j != 0)						m_oscillators[j][0]->addNeighbour(m_oscillators[j - 1][0]);//,m_diffusionCoefficients[j-1][0]*(1-m_anisotrophyCoefficients[j-1][0]));
	//		m_oscillators[j][m_oscillators[j].size() - 1]->addNeighbour(m_oscillators[j][m_oscillators[j].size() - 2]);//,m_diffusionCoefficients[j][m_oscillators[j].size()-2]*m_anisotrophyCoefficients[j][m_oscillators[j].size()-2]);
	//		if (j != m_oscillators.size() - 1)	m_oscillators[j][m_oscillators[j].size() - 1]->addNeighbour(m_oscillators[j + 1][m_oscillators[j].size() - 1]);//,m_diffusionCoefficients[j+1][m_oscillators[j].size()-1]*(1-m_anisotrophyCoefficients[j+1][m_oscillators[j].size()-1]));
	//		if (j != 0)						m_oscillators[j][m_oscillators[j].size() - 1]->addNeighbour(m_oscillators[j - 1][m_oscillators[j].size() - 1]);//,m_diffusionCoefficients[j-1][m_oscillators[j].size()-1]*(1-m_anisotrophyCoefficients[j-1][m_oscillators[j].size()-1]));
	//		
	//		m_oscillators[j][0]->addNeighbour(m_oscillators[j][m_oscillators[j].size() - 1]);//,m_diffusionCoefficients[j][m_oscillators[j].size()-1]*m_anisotrophyCoefficients[j][m_oscillators[j].size()-1]);
	//		m_oscillators[j][m_oscillators[j].size() - 1]->addNeighbour(m_oscillators[j][0]);//,m_diffusionCoefficients[j][0]*m_anisotrophyCoefficients[j][0]);
	//	}
	//}


}
void AtrialMachine2d::editDiffusionCoefficients()
{
	int gridSize = m_grid->m_mesh.size();
	int gridWidth = m_grid->getSize();

	for (int j = 0; j < gridSize; ++j)
	{
		Oscillator *osc = m_grid->m_mesh[j];
		int ii = static_cast<double> (j % gridWidth);
		int jj = floor(static_cast<double>(j) / static_cast<double>(gridWidth));

		osc->setSigma(m_diffusionCoefficients[ii][jj] * m_anisotrophyCoefficients[ii][jj],
			m_diffusionCoefficients[ii][jj] * (1 - m_anisotrophyCoefficients[ii][jj]),
			0);
	}

}
//---------------------------------------------------------------------------------------
void AtrialMachine2d::processStep() 
{
	
	int meshSize = m_grid->m_mesh.size();
	int meshWidth = m_grid->getSize();//m_skip
	if (m_definitions->m_ectopicActivity && m_grid->stimulationBegun == false)
	{
		m_grid->setStimulation(m_grid->m_mesh[m_stimulationID], 5);
	}

	else if (!m_definitions->m_ectopicActivity && m_grid->stimulationBegun == true)
	{
		m_grid->stopStimulation();
	}

	for (int kk = 0; kk <= meshSize; ++kk)
	{ 
		m_globalTime = m_strategy->nextStep();
	}
	RRcalc_1->processNewTime(m_globalTime, probeOscillator[0]->getPotential());
	RRcalc_2->processNewTime(m_globalTime, probeOscillator[1]->getPotential());
	RRcalc_3->processNewTime(m_globalTime, probeOscillator[2]->getPotential());
	m_grid->calculateElectrogram(probeOscillator[0]);
	probeOscillator[0]->stateCalculated(1,0, 1);
	probeOscillator[1]->stateCalculated(1,0, 1);
	probeOscillator[2]->stateCalculated(1,0, 1);
	
	emit emitGlobalTimeOnly(m_globalTime);

}
//-----------------
//--------------------------------------------------------------------
void AtrialMachine2d::setEctoModTime()
{
	m_definitions->setEctoModTime(m_globalTime);
}



void AtrialMachine2d::setSkip(int skip) { 	m_skip = skip; }
//-------------------------
//void AtrialMachine2d::setSaGlobalAlpha(double value)
//{
//	for (int j = 0; j < m_grid->m_mesh.size(); ++j)
//	{
//		if (m_grid->m_mesh[j]->getCellType() == SA_NODE)
//		{
//			m_grid->m_mesh[j]->setParameter(value, VDG_ALPHA);
//		}
//		
//	}
//}
////--
//void AtrialMachine2d::setSaGlobalF(double value)
//{
//	for (int j = 0; j < m_grid->m_mesh.size(); ++j)
//	{
//		if (m_grid->m_mesh[j]->getCellType() == SA_NODE)
//		{
//			m_grid->m_mesh[j]->setParameter(value, VDG_F);
//		}
//
//	}
//}
//void AtrialMachine2d::setSaGlobalD(double value)
//{
//	for (int j = 0; j < m_grid->m_mesh.size(); ++j)
//	{
//		if (m_grid->m_mesh[j]->getCellType() == SA_NODE)
//		{
//			m_grid->m_mesh[j]->setParameter(value, VDG_D);
//		}
//
//	}
//}
//void AtrialMachine2d::setSaGlobalE(double value)
//{
//	for (int j = 0; j < m_grid->m_mesh.size(); ++j)
//	{
//		if (m_grid->m_mesh[j]->getCellType() == SA_NODE)
//		{
//			m_grid->m_mesh[j]->setParameter(value, VDG_E);
//		}
//
//	}
//}
//void AtrialMachine2d::setSaGlobalV1(double value)
//{
//	for (int j = 0; j < m_grid->m_mesh.size(); ++j)
//	{
//		if (m_grid->m_mesh[j]->getCellType() == SA_NODE)
//		{
//			m_grid->m_mesh[j]->setParameter(value, VDG_V1);
//		}
//
//	}
//}
//void AtrialMachine2d::setSaGlobalV2(double value)
//{
//
//	for (int j = 0; j < m_grid->m_mesh.size(); ++j)
//	{
//		if (m_grid->m_mesh[j]->getCellType() == SA_NODE)
//		{
//			m_grid->m_mesh[j]->setParameter(value, VDG_V2);
//		}
//
//	}
//}
//void AtrialMachine2d::setSaGlobalTimeScaler(double value)
//{
	//for (int j = 1; j < m_oscillators.size()-1; ++j)
	//{
	//	for (int i = 1; i <  m_oscillators[j].size()-1; ++i)
	//	{
	//		if(m_grid->m_net[j][i]->m_type == SA_NODE)
	//		{
	//			m_oscillators[j][i]->setParameter( value, VDG_TS );
	//		}
	//	}
	//}
//}
//void AtrialMachine2d::setAvGlobalAlpha(double value)
//{
//	/*for (int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == AV_NODE)
//			{
//				m_oscillators[j][i]->setParameter( value, VDG_ALPHA );
//			}
//		}
//	}*/
//}
//void AtrialMachine2d::setAvGlobalF(double value)
//{
//	//for (int j = 1; j < m_oscillators.size()-1; ++j)
//	//{
//	//	for (int i = 1; i <  m_oscillators[j].size()-1; ++i)
//	//	{
//	//		if(m_grid->m_net[j][i]->m_type == AV_NODE)
//	//		{
//	//			m_oscillators[j][i]->setParameter( value, VDG_F );
//	//		}
//	//	}
//	//}
//}
//void AtrialMachine2d::setAvGlobalD(double value)
//{
//	//for (int j = 1; j < m_oscillators.size()-1; ++j)
//	//{
//	//	for (int i = 1; i <  m_oscillators[j].size()-1; ++i)
//	//	{
//	//		if(m_grid->m_net[j][i]->m_type == AV_NODE)
//	//		{
//	//			m_oscillators[j][i]->setParameter( value, VDG_D );
//	//		}
//	//	}
//	//}
//}
//void AtrialMachine2d::setAvGlobalE(double value)
//{
//	for (int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == AV_NODE)
//			{
//				m_oscillators[j][i]->setParameter( value, VDG_E );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAvGlobalV1(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == AV_NODE)
//			{
//				m_oscillators[j][i]->setParameter( value, VDG_V1 );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAvGlobalV2(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == AV_NODE)
//			{
//				m_oscillators[j][i]->setParameter( value, VDG_V2 );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAvGlobalTimeScaler(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == AV_NODE)
//			{
//				m_oscillators[j][i]->setParameter( value, VDG_TS );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAvBranchGlobalAlpha(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == AV_NODE_BRANCH)
//			{
//				m_oscillators[j][i]->setParameter( value, VDG_ALPHA );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAvBranchGlobalF(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == AV_NODE_BRANCH)
//			{
//				m_oscillators[j][i]->setParameter( value, VDG_F );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAvBranchGlobalD(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == AV_NODE_BRANCH)
//			{
//				m_oscillators[j][i]->setParameter( value, VDG_D );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAvBranchGlobalE(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == AV_NODE_BRANCH)
//			{
//				m_oscillators[j][i]->setParameter( value, VDG_E );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAvBranchGlobalV1(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == AV_NODE_BRANCH)
//			{
//				m_oscillators[j][i]->setParameter( value, VDG_V1 );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAvBranchGlobalV2(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == AV_NODE_BRANCH)
//			{
//				m_oscillators[j][i]->setParameter( value, VDG_V2 );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAvBranchGlobalTimeScaler(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == AV_NODE_BRANCH)
//			{
//				m_oscillators[j][i]->setParameter( value, VDG_TS );
//			}
//		}
//	}
//}
//
//
//void AtrialMachine2d::setAtriumGlobalGamma(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == ATRIAL_TISSUE)
//			{
//				m_oscillators[j][i]->setParameter( value, FHN_GAMMA );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAtriumGlobalBeta(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == ATRIAL_TISSUE)
//			{
//				m_oscillators[j][i]->setParameter( value, FHN_BETA );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAtriumGlobalNi(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == ATRIAL_TISSUE)
//			{
//				m_oscillators[j][i]->setParameter( value, FHN_NI );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAtriumGlobalC1(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == ATRIAL_TISSUE)
//			{
//				m_oscillators[j][i]->setParameter( value, FHN_C1 );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAtriumGlobalC2(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == ATRIAL_TISSUE)
//			{
//				m_oscillators[j][i]->setParameter( value, FHN_C2 );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAtriumGlobalAlpha(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == ATRIAL_TISSUE)
//			{
//				m_oscillators[j][i]->setParameter( value, FHN_ALPHA );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAtriumGlobalTimeScaler(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == ATRIAL_TISSUE)
//			{
//				m_oscillators[j][i]->setParameter( value, FHN_TS );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAtriumBranchGlobalGamma(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == ATRIAL_BRANCH)
//			{
//				m_oscillators[j][i]->setParameter( value, FHN_GAMMA );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAtriumBranchGlobalBeta(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == ATRIAL_BRANCH)
//			{
//				m_oscillators[j][i]->setParameter( value, FHN_BETA );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAtriumBranchGlobalNi(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == ATRIAL_BRANCH)
//			{
//				m_oscillators[j][i]->setParameter( value, FHN_NI );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAtriumBranchGlobalC1(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == ATRIAL_BRANCH)
//			{
//				m_oscillators[j][i]->setParameter( value, FHN_C1 );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAtriumBranchGlobalC2(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == ATRIAL_BRANCH)
//			{
//				m_oscillators[j][i]->setParameter( value, FHN_C2 );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAtriumBranchGlobalAlpha(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == ATRIAL_BRANCH)
//			{
//				m_oscillators[j][i]->setParameter( value, FHN_ALPHA );
//			}
//		}
//	}
//}
//void AtrialMachine2d::setAtriumBranchGlobalTimeScaler(double value)
//{
//	for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
//	{
//		for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
//		{
//			if(m_grid->m_net[j][i]->m_type == ATRIAL_BRANCH)
//			{
//				m_oscillators[j][i]->setParameter( value, FHN_TS );
//			}
//		}
//	}
//}
void AtrialMachine2d::setGlobalTime(double t)
{
	m_globalTime = t;

}
//void AtrialMachine2d::setAtriumV3multi(double value)
//{
	//for (unsigned int j = 1; j < m_oscillators.size()-1; ++j)
	//{
	//	for (unsigned int i = 1; i <  m_oscillators[j].size()-1; ++i)
	//	{
	//		if(m_grid->m_net[j][i]->m_type == ATRIAL_V3)
	//		{
	//			m_oscillators[j][i]->setParameter( value, V3_CRN );
	//		}
	//	}
	//}
//}









////ELECTROGRAM
//int i3 = static_cast<int>((probeOscillator[0]->getPositionX()));
//int j3 = static_cast<int>((probeOscillator[0]->getPositionY()));
//double ele_val = 0;
//if (1)
//{
//	Oscillator *osc;
//	for (int j2 = 0; j2 < m_oscillators.size(); ++j2)//12; ++j)//
//	{
//		for (int i2 = 0; i2 < m_oscillators.size(); ++i2)
//		{
//			osc = m_oscillators[j2][i2];

//			if (osc->getCellType() != SOLID_WALL)
//			{
//				double val = osc->getExtrapolatedNeighbourSource();
//				val = val / (pow((osc->getPositionX() - probeOscillator[0]->getPositionX()), 2) +
//					pow(osc->getPositionY() - probeOscillator[0]->getPositionY(), 2) + 0.5);
//				ele_val = ele_val += val;
//			}
//		}

//	}
//}
//probeOscillator[0]->setElectrogram(ele_val);
//RRcalc_1->processNewTime(m_globalTime, ele_val);