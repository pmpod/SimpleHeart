#pragma once
#include "CardiacMesh.h"

class NumericStrategy
{
public:
	NumericStrategy(CardiacMesh* oscillators);
	~NumericStrategy();

	virtual double nextStep() = 0;
	virtual void reset() = 0;

	CardiacMesh* m_mesh;
};

