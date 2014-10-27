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
	 


	/**
	* \brief Calculates a single step of simulation. 
	*/
	virtual double nextStep() = 0;  ///< Main function

	/**
	* \brief For non-uniform timestep methods synchronises time in all calculation cells; For fixed timestep does nothing
	*/
	virtual double synchronise() = 0;  ///< Main function


	/**
	* \brief Resets all the Oscillators to the resting state and the time to 0
	*/
	virtual void reset();

	/**
	* \brief Performs a defibrillation shot (under construction)
	*/
	void defibrillate();


	/**
	* \brief A pointer to the model
	*/
	CardiacMesh* m_mesh;

};

