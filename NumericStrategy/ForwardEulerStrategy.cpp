#include "NumericStrategy\ForwardEulerStrategy.h"


ForwardEulerStrategy::ForwardEulerStrategy(CardiacMesh* oscillators) : NumericStrategy(oscillators)
{
	m_mainTimestep = 0.08;
}

double ForwardEulerStrategy::nextStep()
{
	Oscillator *osc;
	double nextTimestep = m_mainTimestep;
	int numberOfEquations;
	int meshSize = m_mesh->m_mesh.size();

	for (int j = 0; j < meshSize; ++j)
	{
		osc = m_mesh->m_mesh[j];
		osc->m_previousTime = osc->m_currentTime;
		osc->m_currentTime += nextTimestep;
		if (osc->getCellType() != SOLID_WALL)
		{

			numberOfEquations = m_mesh->m_mesh[j]->getNumberOfCurrents();
			//SCA�KUJ POTENCJA�
			osc->m_previous_potential = osc->m_v_potential;
			osc->m_v_potential += nextTimestep * (osc->getPotentialPrim() + osc->getUniformTimestepCurrentSource());

			osc->m_v_scaledPotential = osc->vzero + (osc->vmax - osc->vmin)*osc->m_v_potential;
			osc->m_previous_scaledPotential = osc->vzero + (osc->vmax - osc->vmin)*osc->m_previous_potential;
			osc->m_v_electrogram = osc->m_v_potential;
			//SCA�KUJ WSZYSTKIE PR�DY
			for (short k = 0; k < numberOfEquations; ++k)
			{
				osc->m_v_current[k] += nextTimestep * (osc->getCurrentPrim(k));
			}
			//osc->m_lastHistoryIterator = (osc->m_lastHistoryIterator + 1) % OSC_HISTORY_SIZE;
			//osc->m_potentialHistory[osc->m_lastHistoryIterator] = osc->m_v_scaledPotential;
			if (!osc->m_wallCells.empty())
			{
				for (short wc = 0; wc < osc->m_wallCells.size(); ++wc)
				{
					osc->m_wallCells[wc]->m_v_potential = osc->m_v_potential;
					osc->m_wallCells[wc]->m_v_scaledPotential = osc->m_v_scaledPotential;
					osc->m_wallCells[wc]->m_previous_potential = osc->m_previous_potential;
					osc->m_wallCells[wc]->m_previous_scaledPotential = osc->m_previous_scaledPotential;

					numberOfEquations = osc->m_wallCells[wc]->getNumberOfCurrents();
					for (int k = 0; k < numberOfEquations; ++k)
					{
						if (osc->getNumberOfCurrents()<k)
							osc->m_wallCells[wc]->m_v_current[k] = osc->m_v_current[k];
					}
				}
			}	
			m_mesh->processActivationTime(osc);
		}

	}
	m_mesh->m_simulationTime = osc->m_currentTime;

	return (osc->m_currentTime);
}
ForwardEulerStrategy::~ForwardEulerStrategy()
{
}
