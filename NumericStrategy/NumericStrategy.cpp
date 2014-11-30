#include "NumericStrategy\NumericStrategy.h"


NumericStrategy::NumericStrategy(CardiacMesh* oscillators)
{
	m_mesh = oscillators;
	m_mesh->m_simulationTime = 0;
}


NumericStrategy::~NumericStrategy()
{
}

void NumericStrategy::defibrillate()
{
	reset();
}

void NumericStrategy::reset()
{
	int gridSize = m_mesh->m_mesh.size();
	m_mesh->m_simulationTime = 0;

	for (int j = 0; j < gridSize; ++j)
	{
		Oscillator *osc = m_mesh->m_mesh[j];
		osc->reset();
	}
	
}

