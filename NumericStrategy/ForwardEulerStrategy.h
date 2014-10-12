#pragma once
#include "NumericStrategy\NumericStrategy.h"

class ForwardEulerStrategy :
	public NumericStrategy
{
public:
	ForwardEulerStrategy(CardiacMesh* oscillators);
	~ForwardEulerStrategy();

	double nextStep();
	void reset();

	double m_mainTimestep;
};

