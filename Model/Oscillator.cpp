#include "Oscillator.h"
#include <boost\foreach.hpp>
#include <cmath>
Oscillator::Oscillator(void)
{
	m_type = NONE;
	m_v_current.push_back(0.0);
	m_currentPRIM.push_back(0.0);

	reset();
	m_farthestDistanceID = -1;
	m_closestDistanceID = -1;
	vzero = -75; //[mV]
	vmax = 25;  //[mV]
	vmin = -75;  //[mV]
	m_underStimulation = false;
}

void Oscillator::reset()
{
	m_stimulation = 0.0;
	m_v_potential = 0.0;
	m_v_scaledPotential = 0.0;
	m_previous_potential = 0.0;
	m_previous_scaledPotential = 0.0;
	m_potentialPRIM = 0.0;
	m_v_electrogram = 0.0;
	m_currentSource = 0.0;
	m_previousTime = -0.1;
	m_currentTime = 0.0;
	_sourceA = 0.0;
	_sourceB = 0.0;
	guardTimestep = 0.4;
	m_underStimulation = false;
	for (short k = 0; k < m_v_current.size(); ++k)
	{
		m_v_current[k] = 0.0;
		m_currentPRIM[k] = 0.0;
	}
	m_currentTime = 0.0;
	m_previousTime = -0.1;
}
//--------------------------------------------------------------
Oscillator::~Oscillator(void)
{
	m_v_current.clear();
	//TODO empty all fields
}
//--------------------------------------------------------------
double Oscillator::getCurrent(const int&which)
{
	return m_v_current[which];
}
//--------------------------------------------------------------
double Oscillator::getPotential()
{ 
	return m_v_scaledPotential;
}
//--------------------------------------------------------------
double Oscillator::getPreviousPotential()
{
	return (m_previous_potential - vzero) / (vmax - vmin);
}
//--------------------------------------------------------------
double& Oscillator::getRefPotential(){ return m_v_scaledPotential; }				//zwraca m_v_potential
double& Oscillator::getRefPreviousPotential() { return m_previous_scaledPotential; }				//zwraca m_v_potential
double& Oscillator::getRefCurrentTime(){ return m_currentTime; }
//--------------------------------------------------------------
double Oscillator::getCurrentTime(){ return m_currentTime; }
//--------------------------------------------------------------
double Oscillator::getPreviousTime(){ return m_previousTime; }
//--------------------------------------------------------------
double Oscillator::getPotentialPrim()
{
	m_potentialPRIM = -2 * m_v_current[0] + m_stimulation;
	return m_potentialPRIM;
}
//--------------------------------------------------------------
double Oscillator::getCurrentPrim(const int& which)
{
	if (which == 1)
		m_currentPRIM[0] = -2 * m_v_potential;
		return m_currentPRIM[0];

	return 0;
}
//--------------------------------------------------------------
CELL_TYPE Oscillator::getCellType() {	return m_type; }
//--------------------------------------------------------------
double Oscillator::getElectrogram() { return m_v_electrogram; }
//---------------------------------------------------------------
double Oscillator::getPositionX()  { return m_x; }
//---------------------------------------------------------------
double Oscillator::getPositionY() { return m_y; }
//---------------------------------------------------------------
double Oscillator::getPositionZ() { return m_z; }
//--------------------------------------------------------------
void Oscillator::setCurrent(const double& current, const int& which)
{
	m_v_current[which] = current;
}
//--------------------------------------------------------------
void Oscillator::setPotential(const double& potential)
{
	m_v_scaledPotential = potential;
	m_v_potential = vzero + (vmax - vmin)*potential;

	_sourceA = (m_v_scaledPotential - m_previous_scaledPotential) / (m_currentTime - m_previousTime);
	_sourceB = m_v_scaledPotential - _sourceA*m_currentTime;

}
void Oscillator::setPreviousPotential(const double& potential)
{
	m_previous_scaledPotential = potential;
	m_previous_potential = vzero + (vmax - vmin)*potential;
}
//--------------------------------------------------------------
void Oscillator::setElectrogram(const double&  electrogram)
 {
	 m_v_electrogram=electrogram;
 }
//--------------------------------------------------------------
void Oscillator::setParameter( double value, OSC_PARAMETER parameter )
{
	//virtual function to be defined in particular oscillator
}
//--------------------------------------------------------------
void Oscillator::setSigma(double sX, double sY, double sZ)
{
	m_sigmaX = sX;
	m_sigmaY = sY;
	m_sigmaZ = sZ;

	setConnexins();
}
void Oscillator::setConnexins()
{
	m_ConnexinSum = 0;
	////Aim at setting anisotropic connexin
	//for (unsigned int i = 0; i < m_neighbours.size(); ++i)
	//{
	//	double deltaRX = std::abs(m_neighbours[i]->getPositionX() - m_x);
	//	double deltaRY = std::abs(m_neighbours[i]->getPositionY() - m_y);
	//	double deltaRZ = std::abs(m_neighbours[i]->getPositionZ() - m_z);

	//	double deltaR = sqrt(std::pow(deltaRX, 2) + std::pow(deltaRY, 2) + std::pow(deltaRZ, 2));

	//	double sigma = m_sigmaX * (deltaRX / deltaR);
	//	sigma += m_sigmaY * (deltaRY / deltaR);
	//	sigma += m_sigmaZ * (deltaRZ / deltaR);

	//	m_connexin[i] = sigma / std::pow(deltaR, 2);
	//	m_ConnexinSum += m_connexin[i];
	//}

	//TODO: Change into anisotropic conduction handling
	for (unsigned int i = 0; i < m_neighbours.size(); ++i)
	{
		double deltaRX = std::abs(m_neighbours[i]->getPositionX() - m_x);
		double deltaRY = std::abs(m_neighbours[i]->getPositionY() - m_y);
		double deltaRZ = std::abs(m_neighbours[i]->getPositionZ() - m_z);

		double deltaR = sqrt(std::pow(deltaRX, 2) + std::pow(deltaRY, 2) + std::pow(deltaRZ, 2));

		double sigma = m_sigmaX;// *(deltaRX / deltaR);
		//sigma += m_sigmaY * (deltaRY / deltaR);
		//sigma += m_sigmaZ * (deltaRZ / deltaR);

		m_connexin[i] = sigma / std::pow(deltaR, 2);
		m_ConnexinSum += m_connexin[i];
	}
}
//--------------------------------------------------------------
void Oscillator::addNeighbour(Oscillator* neighbour)
{
	double deltaRX = std::abs(neighbour->getPositionX() - m_x);
	double deltaRY = std::abs(neighbour->getPositionY() - m_y);
	double deltaRZ = std::abs(neighbour->getPositionZ() - m_z);

	double deltaR = sqrt(std::pow(deltaRX, 2) + std::pow(deltaRY, 2) + std::pow(deltaRZ, 2));


	if (m_farthestDistanceID == -1)
		m_farthestDistanceID = deltaR;
	if (m_farthestDistanceID < deltaR)
		m_farthestDistanceID = deltaR;

	if (m_closestDistanceID == -1)
		m_closestDistanceID = deltaR;
	if (m_closestDistanceID > deltaR)
		m_closestDistanceID = deltaR;

	m_neighbours.push_back(neighbour);
	m_neighboursDistance.push_back(deltaR);
	m_connexin.push_back(0.0);
	void setConnexins();

}
//--------------------------------------------------------------
double Oscillator::getExtrapolatedNeighbourSource()
{
	m_currentSource = 0;

	Oscillator* neigh;
	unsigned short nsize = m_neighbours.size();

	for (unsigned short i = 0; i < nsize; ++i)
	{
		neigh = m_neighbours[i];
		//m_currentSource += (neigh->getPotential() + ((neigh->getPotential() - neigh->getPreviousPotential())* (m_currentTime - neigh->getCurrentTime()) / (neigh->getCurrentTime() - neigh->getPreviousTime()))) *m_connexin[i];
		m_currentSource += (neigh->_sourceA*m_currentTime + neigh->_sourceB)*m_connexin[i];
	}
	
	return  m_currentSource;
}
//--------------------------------------------------------------
double Oscillator::getUniformTimestepCurrentSource()
{
	double mixed = 0;
	double Vinterpol = 0;
	for (unsigned int i = 0; i < m_neighbours.size(); ++i)
	{
		if (m_currentTime == (m_neighbours[i]->getCurrentTime()))
			Vinterpol = m_neighbours[i]->getPreviousPotential();
		else
			Vinterpol = m_neighbours[i]->getPotential();

		mixed += (Vinterpol - m_v_potential) *m_connexin[i];
	}

	m_currentSource = mixed;
	return m_currentSource;
}
//--------------------------------------------------------------
double Oscillator::getLastCurrentSource()
{
	double mixed = 0;
	for (unsigned int i = 0; i < m_neighbours.size(); ++i)
	{
			mixed += (m_neighbours[i]->getPotential() - m_v_potential) *m_connexin[i];
	}

	m_currentSource = mixed;
	return m_currentSource;
}
//--------------------------------------------------------------
//double Oscillator::getLastCurrentSource(){ return m_currentSource; }
//--------------------------------------------------------------
int Oscillator::getNumberOfCurrents(){ return this->m_v_current.size(); }
//---------------------------------------------------------------
void Oscillator::setPositionX(const double& pos)
{
	m_x = pos;
}
void Oscillator::setPositionY(const double& pos)
{
	m_y = pos;
}
void Oscillator::setPositionZ(const double& pos)
{
	m_z = pos;
}
//---------------------------------------------------------------
void Oscillator::setCurrentTime(const double& time)
{
	if (time == 0.0)
	{
		m_previousTime = -0.1;
	}
	else
	{
		m_previousTime = m_currentTime;
	}
	m_currentTime = time;
}
//----------------
void Oscillator::stateCalculated(double a, double b, int which)
{
	emit newPotentialValue(a*getElectrogram() + b);
	emit newState(a*getElectrogram(), a*getCurrent(which) + b);
	emit newPotentialTime(m_currentTime, getElectrogram());
}
void Oscillator::setType(CELL_TYPE type)
{
	m_type = type;
}