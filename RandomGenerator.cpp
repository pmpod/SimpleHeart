#include "RandomGenerator.h"
#include <cmath>
#include <ctime>
#include <cstdlib>
RandomGenerator::RandomGenerator(void)
{
	srand( 17 ); //(unsigned) time(0));	//start silnika rand
}
//---------------------------------------------------------------------------
RandomGenerator::~RandomGenerator(void)
{
}
//---------------------------------------------------------------------------
double RandomGenerator::gaussRandom(double mean, double sigma)
{
	double u1;
	double u2;

	while(true)
	{
		u1 = 2*uniform();
		u2 = uniform() * 6 * sigma - 3 * sigma + mean;;
		if (u1 < gaussFunction(u2, mean, sigma))
		{
			return u2;
		}
	}
}
//---------------------------------------------------------------------------
double RandomGenerator::gaussFunction(double y, double mean, double sigma)
{
	double norm = 1/sqrt( 2 * 3.14) * sigma ;
	double expon = exp( ( -pow( ( y - mean ), 2 ) ) / (2 * pow( sigma, 2 ) ) );
	
	return norm*expon;
}
//---------------------------------------------------------------------------
int RandomGenerator::flipper(int maxNumber)
{
	return (int((maxNumber+1)*rand()/(RAND_MAX + 1.0)));
}
//---------------------------------------------------------------------------
double RandomGenerator::uniform()
{
	return ((double)rand() / ((double)(RAND_MAX)+(double)(1)));
}
//---------------------------------------------------------------------------