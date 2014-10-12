#pragma once
#include <cmath>
class RandomGenerator
{
public:
	RandomGenerator(void);
	~RandomGenerator(void);

	double gaussFunction(double y, double mean, double sigma);
	double gaussRandom(double mean, double sigma);
	double uniform();
	int flipper( int maxNumber);
};
