#include "NumericStrategy\NumericStrategy.h"


NumericStrategy::NumericStrategy(CardiacMesh* oscillators)
{
	m_mesh = oscillators;
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

	for (int j = 0; j < gridSize; ++j)
	{
		Oscillator *osc = m_mesh->m_mesh[j];
		osc->reset();
	}
	
}