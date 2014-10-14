#include "NumericStrategy\ForwardEulerStrategy.h"


ForwardEulerStrategy::ForwardEulerStrategy(CardiacMesh* oscillators) : NumericStrategy(oscillators)
{
	reset();
}

void ForwardEulerStrategy::reset()
{
	m_mainTimestep = 0.1;
}

double ForwardEulerStrategy::nextStep()
{
	Oscillator *osc;
	double nextTimestep = m_mainTimestep;
	double nextTime ;
	double nextPotential = 0;
	double nextCurrent[] = { 0, 0, 0 };
	int numberOfEquations = m_mesh->m_mesh[0]->getNumberOfCurrents();

	int meshSize = m_mesh->m_mesh.size();
	int meshWidth = m_mesh->getSize();



	for (int j = 0; j < meshSize; ++j)
	{
		osc = m_mesh->m_mesh[j];

		if (osc->getCellType() != SOLID_WALL)
		{

			if (osc->m_underStimulation == false)
			{
				//SCA£KUJ POTENCJA£
				osc->setPreviousPotential(osc->m_v_potential);
				osc->setPotential(osc->m_v_potential + nextTimestep * (osc->getPotentialPrim() + osc->getUniformTimestepCurrentSource()));
				osc->setElectrogram(osc->m_v_potential);
				//SCA£KUJ WSZYSTKIE PR¥DY
				for (int k = 0; k < numberOfEquations; ++k)
				{
					osc->setCurrent(osc->m_v_current[k] + nextTimestep * (osc->getCurrentPrim(k)), k);
				}
			}
			else
			{
				osc->setPreviousPotential(osc->m_v_potential);
				osc->setPotential(m_mesh->m_ectopicAmplitude);
			}

			nextTime = (osc->getCurrentTime()) + nextTimestep;
			osc->setCurrentTime(nextTime);
		}

	}

	//border walls
	for (int i = 0; i < m_mesh->m_wallCells.size(); ++i)
	{
		m_mesh->m_wallCells[i].first->setPotential(m_mesh->m_wallCells[i].second->getPotential());
		m_mesh->m_wallCells[i].first->setElectrogram(m_mesh->m_wallCells[i].second->getElectrogram());
		m_mesh->m_wallCells[i].first->setPreviousPotential(m_mesh->m_wallCells[i].second->getPreviousPotential());
		for (int k = 0; k < numberOfEquations; ++k)
		{
			m_mesh->m_wallCells[i].first->setCurrent(m_mesh->m_wallCells[i].second->getCurrent(k), k);
		}

		m_mesh->m_wallCells[i].first->setCurrentTime(m_mesh->m_wallCells[i].second->getCurrentTime());
	}

	return (nextTime);
}
ForwardEulerStrategy::~ForwardEulerStrategy()
{
}
