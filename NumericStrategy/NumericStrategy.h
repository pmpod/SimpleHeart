#pragma once
#include "Model\CardiacMesh.h"


/*! \class NumericStrategy
* \brief NumericStrategy is a main interface class for the numerical integration of the model's equations. 
* It provides the interface for numerical schemes like Forward Euler Scheme, modified Backward Euler Scheme, Allexandre&Otani scheme etc.
*
* The classes that implement a concrete numerical calculation strategy should implement this.
* The XXX AtrialMachine Class uses this to call the concrete numerical scheme to integrate electrophysiological equations of a given model.
*/
class NumericStrategy
{
public:
	/**
	* \brief A NumericStrategy constructor.
	* @param oscillators - a pointer to the already initialized CardiacMesh model
	*/
	NumericStrategy(CardiacMesh* oscillators);
	/**
	* \brief A NumericStrategy destructor.
	*/
	~NumericStrategy();
	 
	virtual double nextStep() = 0;  ///< Main function
	virtual void reset() = 0;

	CardiacMesh* m_mesh;
};
