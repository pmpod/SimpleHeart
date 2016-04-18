#include "GridElectrode.h"
#include <stdlib.h>


#include "Control/epStimulator.h"

GridElectrode::GridElectrode()
{
}

GridElectrode::GridElectrode(int spacing, int x, int y, int nx, int ny)
{
	_spacingXY = spacing; // distance between electrodes in oscillators
	_numberOfElectrodesX = nx; // distance between electrodes in oscillators
	_numberOfElectrodesY = ny; // distance between electrodes in 
	posX = x; //lefttop electrode position
	 posY = y;//lefttop electrode position
}

GridElectrode::~GridElectrode()
{
}
void GridElectrode::reset()
{

	for (int j = 0; j < m_historyElectrogram.size(); ++j)
	{
		m_historyElectrogram[j].clear();
	}
	m_historyElectrogram.clear();

	for (int j = 0; j < m_historyActivationTimes.size(); ++j)
	{
		m_historyActivationTimes[j].clear();
	}
	m_historyActivationTimes.clear();

	for (int j = 0; j < m_historyAfterHilbert.size(); ++j)
	{
		m_historyAfterHilbert[j].clear();
	}
	m_historyAfterHilbert.clear();

	for (int j = 0; j < m_historyTime.size(); ++j)
	{
		m_historyTime[j].clear();
	}
	m_historyTime.clear();

	for (int j = 0; j < m_historyPotential.size(); ++j)
	{
		m_historyPotential[j].clear();
	}
	m_historyPotential.clear();

	

}

void GridElectrode::calculateHilbert()
{
	for (int j = 0; j < m_historyElectrogram.size(); ++j)
	{
		m_historyAfterHilbert[j] = library.get_Hilbert_phase_using_convolution_double(& m_historyElectrogram[j]);
	}
}
