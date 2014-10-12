#include "ForwardEulerStrategy.h"


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
	double nextPotential = 0;
	double nextCurrent[] = { 0, 0, 0 };
	int numberOfEquations = m_mesh->m_mesh[0]->getNumberOfCurrents();

	int meshSize = m_mesh->m_mesh.size();
	int meshWidth = m_mesh->getSize();



	for (int j = 0; j < meshSize; ++j)//12; ++j)//
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

			osc->setCurrentTime((osc->getCurrentTime()) + nextTimestep);

		}



	}
	return (osc->getCurrentTime()) + nextTimestep;
}
ForwardEulerStrategy::~ForwardEulerStrategy()
{
}
