#pragma once
#include "NumericStrategy\NumericStrategy.h"

class ForwardEulerStrategy :
	public NumericStrategy
{
public:
	ForwardEulerStrategy(CardiacMesh* oscillators);
	~ForwardEulerStrategy();

	double nextStep();
	double synchronise() { return m_mesh->m_simulationTime; };

	double m_mainTimestep;
};

