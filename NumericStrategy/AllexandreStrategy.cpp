#include "NumericStrategy\AllexandreStrategy.h"
#include <algorithm>
#define EPSILONSTEP 0.000001

AllexandreStrategy::AllexandreStrategy(CardiacMesh* oscillators) : NumericStrategy(oscillators)
{
	//[0] Set parameters
	deltaTimestepIncMean = oscillators->getDeltaR()*oscillators->getDeltaR() / (2 * oscillators->m_maximumCV);//
	deltaTdiff = oscillators->getDeltaR() * oscillators->getDeltaR();
	guardUpdateTimestep = oscillators->m_maximumCV / oscillators->getDeltaR();
	kappaAccuracy = 0.05;
	Kguard = 2;

	//[1] Create time trees
	m_updateTimeTree = nullptr;
	constructUpdateTimeTree();
	constructTimestepTree();

	//[2] Initialize transient parameters

	latestTime = 0;
	lastGuardUpdate = -guardUpdateTimestep - 1;
	resetTime(m_updateTimeTree, 0.0);

	stimulationStateUpdate = false;
	guardCellUpdate();

}
//---------------------------------------------------------------------------------
void AllexandreStrategy::reset()
{
	NumericStrategy::reset();
	latestTime = 0;
	lastGuardUpdate = -guardUpdateTimestep-1;

	stimulationStateUpdate = false;
	resetTime(m_updateTimeTree, 0.0);
	guardCellUpdate();
}
AllexandreStrategy::~AllexandreStrategy()
{
	clear(m_updateTimeTree);
	timeStepHeap->clear();
	otherHeap->clear();
	m_oscillatorTimestepDictionary.clear();
}
double AllexandreStrategy::synchronise()
{
	bool synchronised = false;

	while (!synchronised)
	{
		synchronised = true;
		//------------------------------------------------------
		//[1] Find earliest node
		TimeTree* currentNode = goToEarliest(m_updateTimeTree);
		if (currentNode->time < latestTime)
		{
			synchronised = false;
		}
		else if ((currentNode->time - latestTime) > EPSILONSTEP)
		{
			latestTime = currentNode->time;
			synchronised = false;
		}

		if (!synchronised)
		{
			//[4] Get the oscillator and calculate new timestep
			Oscillator* osc = currentNode->osc;
			double lastLocalTimestep = osc->m_currentTime - osc->m_previousTime;
			//[1] determine Next Max Local Timestep Method2b

			double bestLocalTimestep = getMaxTimestep_Method2b((osc->m_v_potential - osc->m_previous_potential) / lastLocalTimestep, osc->m_currentPRIM[0], lastLocalTimestep* (osc->m_connexin[0] * (osc->vmax - osc->vmin)));
			//TODO is this model depndent? It should be model independent quantity / and osc->m_connexin[0] is bad

			//[2] Check if the timestep exceeds the guard value
			if (bestLocalTimestep > osc->guardTimestep)
				nextTimestep = osc->guardTimestep;
			else
				nextTimestep = bestLocalTimestep;


			//[2] Check if the timestep exceeds the latestTim e
			if (osc->m_currentTime + nextTimestep > latestTime)
				nextTimestep = latestTime - osc->m_currentTime;

			if (nextTimestep < EPSILONSTEP)
				nextTimestep = EPSILONSTEP;

			stepModifiedBackwardEuler(osc, nextTimestep);
		

			///////////////////////////////////////
			//[5] Update the time update tree
			bubbleNewTime(currentNode, osc->m_currentTime);

			//[6] Update the timestep set
			(*(m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle))->timestep = bestLocalTimestep;

			if (lastLocalTimestep > bestLocalTimestep)
				timeStepHeap->increase(m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle);
			else
				timeStepHeap->update(m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle);
			
		}
	}
	guardCellUpdate();
	return latestTime;
}
//---------------------------------------------------------------------------------
double AllexandreStrategy::nextStep()
{
	//[0] Check for the stimulation going on
	if (m_mesh->stimulationBegun && !stimulationStateUpdate)
	{
		double dTime;
		//update potentials in all stimulated cells
		unsigned short stimulatedSize = m_mesh->m_underStimulation.size();
		for (unsigned short k = 0; k < stimulatedSize; ++k)
		{
			Oscillator* osc = m_mesh->m_underStimulation[k];
			if (osc->m_type != SOLID_WALL)
			{
				dTime = min(deltaTdiff / (2 * osc->m_connexin[0]*(osc->vmax - osc->vmin)), 0.9999*(timeStepHeap->top())->timestep);
				//TODO connexin [0]
				stepModifiedBackwardEuler(osc, dTime);
				//bubble time
				bubbleNewTime(m_oscillatorUpdateDictionary[osc->oscillatorID].p_updateTimeTreeNode, osc->m_currentTime);

				// Update the timestep set			
				(*(m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle))->timestep = dTime;
				timeStepHeap->update(m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle);
			}
		}
		guardCellUpdate();
		stimulationStateUpdate = true;
	}
	else if (!m_mesh->stimulationBegun && stimulationStateUpdate)
	{
		stimulationStateUpdate = false;
	}
	//------------------------------------------------------
	//[1] Find earliest node
	TimeTree* currentNode = goToEarliest(m_updateTimeTree);

	//[4] Get the oscillator and calculate new timestep
	Oscillator* osc = currentNode->osc;
	v_prim = (osc->m_v_potential - osc->m_previous_potential);

	//[1] determine Next Max Local Timestep Method2b
	double lastLocalTimestep = osc->m_currentTime - osc->m_previousTime;
	double bestLocalTimestep = getMaxTimestep_Method2b(v_prim / lastLocalTimestep, osc->m_currentPRIM[0], lastLocalTimestep* (osc->m_connexin[0] * (osc->vmax - osc->vmin)));
	//TODO is this model depndent? It should be model independent quantity / and osc->m_connexin[0] is bad

	//[2] Check if the timestep exceeds the guard value
	if (bestLocalTimestep > osc->guardTimestep)
		nextTimestep = osc->guardTimestep;
	else
		nextTimestep = bestLocalTimestep;

	stepModifiedBackwardEuler(osc, nextTimestep);


	//[5] Update the time update tree
	bubbleNewTime(currentNode, osc->m_currentTime);

	m_mesh->m_simulationTime = currentNode->time;
	if (m_mesh->m_simulationTime  > latestTime)
		latestTime = m_mesh->m_simulationTime;
	m_mesh->m_simulationTime = latestTime;
	//[2] Update gurad cell algorithm if necessary
	if (latestTime - lastGuardUpdate >= guardUpdateTimestep)
	{
		guardCellUpdate();
		lastGuardUpdate = latestTime;
	}

	//[6] Update the timestep set
	(*(m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle))->timestep = bestLocalTimestep;

	if (lastLocalTimestep > bestLocalTimestep)
		timeStepHeap->increase(m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle);
	else
		timeStepHeap->update(m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle);

	return m_mesh->m_simulationTime;
}
double AllexandreStrategy::stepModifiedBackwardEuler(Oscillator * osc, double nextTimestep)
{

	//[2] Set new current time on oscillator
	osc->m_previousTime = osc->m_currentTime;
	osc->m_currentTime += nextTimestep;

	int numberOfEquations = osc->getNumberOfCurrents();
	//[3] Calculate new step 

		osc->m_previous_potential = osc->m_v_potential;
		v_prim = osc->m_v_potential + nextTimestep * (osc->getPotentialPrim() + osc->getExtrapolatedNeighbourSource());
		v_prim = v_prim / (1 + nextTimestep * osc->m_ConnexinSum);
		osc->m_v_potential = v_prim;

		osc->m_v_scaledPotential = osc->vzero + (osc->vmax - osc->vmin)*osc->m_v_potential;
		osc->m_previous_scaledPotential = osc->vzero + (osc->vmax - osc->vmin)*osc->m_previous_potential;

		osc->_sourceA = (osc->m_v_scaledPotential - osc->m_previous_scaledPotential) / nextTimestep;
		osc->_sourceB = osc->m_v_scaledPotential - osc->_sourceA*osc->m_currentTime;

		for (short k = 0; k < numberOfEquations; ++k)
		{
			osc->m_v_current[k] += nextTimestep * (osc->getCurrentPrim(k));
		}
		//osc->m_v_electrogram = osc->m_v_potential;
		
		//osc->setElectrogram(nextTimestep);

	if (!osc->m_wallCells.empty())
	{
		for (short wc = 0; wc < osc->m_wallCells.size(); ++wc)
		{
			osc->m_wallCells[wc]->m_v_potential = osc->m_v_potential;
			osc->m_wallCells[wc]->m_v_scaledPotential = osc->m_v_scaledPotential;
			osc->m_wallCells[wc]->m_previous_potential = osc->m_previous_potential;
			osc->m_wallCells[wc]->m_previous_scaledPotential = osc->m_previous_scaledPotential;
			osc->m_wallCells[wc]->_sourceA = osc->_sourceA;
			osc->m_wallCells[wc]->_sourceB = osc->_sourceB;

			osc->m_wallCells[wc]->m_v_electrogram = osc->m_v_electrogram;
			for (int k = 0; k < numberOfEquations; ++k)
			{
				osc->m_wallCells[wc]->m_v_current[k] = osc->m_v_current[k];
			}
			
			if (osc->getCurrentTime() != osc->m_wallCells[wc]->m_currentTime)
				osc->m_wallCells[wc]->m_currentTime = osc->m_currentTime;
		}
	}

	return 1;
}
double AllexandreStrategy::guardCellUpdate()
{
	TimestepNode* node;
	Oscillator* osc;
	Oscillator* nosc;
	stpHeap::handle_type handle;
	double Dt;
	double updateTime;
	int counter = 0;
	while (!timeStepHeap->empty())
	{
		//[1] Take the top node
		node = timeStepHeap->top();
		osc = node->osc;

		//[1] Update the guard Timestep based on the node timestep
		osc->guardTimestep = node->timestep;

		//[2] Prepare timestep value to update neighbours
		v_prim = (osc->m_v_potential - osc->m_previous_potential) / (osc->m_currentTime- osc->m_previousTime);
		updateTime = node->timestep + getDeltaTimestepInc(v_prim);

		//[3] Update neighbours
		short numberOfNeighs = osc->m_neighbours.size();
		for (short k = 0; k < numberOfNeighs; ++k)
		{
			nosc = osc->m_neighbours[k];
			handle = m_oscillatorTimestepDictionary[nosc->oscillatorID].timestepHandle;

			//Check, if the neighbour is still in the current heap
			if ((m_oscillatorTimestepDictionary[nosc->oscillatorID].inHeap == currentHeap) && 
				nosc->getCellType() != SOLID_WALL && nosc->getCellType() != NONE)
			{
				if ((*handle)->timestep > updateTime) 
				{
					//[A] update current timestep in the node
					(*handle)->timestep = updateTime;
					timeStepHeap->increase(handle);
					//[B] update guardTimestep
					nosc->guardTimestep = updateTime;
				}
				else
				{
					nosc->guardTimestep = (*handle)->timestep;
					timeStepHeap->increase(handle);
				}
			}

		}
		//[4] Pop the node from the current heap
		timeStepHeap->pop();
		//[5] Insert the node into the new heap & update the dictionary
		m_oscillatorTimestepDictionary[osc->oscillatorID].inHeap = (!currentHeap);
		m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle = otherHeap->push(node);
	}
	std::swap(timeStepHeap, otherHeap);
	currentHeap = !currentHeap;
	return 1;
}
//---------------------------------------------------------------------------------
double AllexandreStrategy::getDeltaTimestepInc( double& v_prim)
{
	v_prim = abs(v_prim);
	if (v_prim >= 0.1 / Kguard)
		return deltaTimestepIncMean / (1 + Kguard*v_prim);
	else
		return deltaTimestepIncMean;
}
//---------------------------------------------------------------------------------
double AllexandreStrategy::getMaxTimestep_Method2b(const double& v_prim, const double& fast_prim, const double& Kd)
{
	double f = kappaAccuracy / ((1 + Kd / 2) * (abs(v_prim) + abs(fast_prim)));
	if (f > 5)	//STYMULACJA NAPRAWIC TO!!!!!!!
		f = 5;
	return f;
}
//---------------------------------------------------------------------------------
void AllexandreStrategy::constructUpdateTimeTree()
{
	if (m_mesh->m_mesh.empty())
		return;

	clear(m_updateTimeTree);
	m_updateTimeTree = newTreeNode();
	newTreeTrunk(m_updateTimeTree, m_mesh->m_mesh, m_oscillatorUpdateDictionary);
}
//---------------------------------------------------------------------------------
void AllexandreStrategy::constructTimestepTree()
{
	if (m_mesh->m_mesh.empty())
		return;
	heap0.clear();
	heap1.clear();
	stpHeap::handle_type handle;
	for (int i = 0; i < m_mesh->m_mesh.size(); ++i)
	{

		if (m_mesh->m_mesh[i]->getCellType() != SOLID_WALL && m_mesh->m_mesh[i]->getCellType() != NONE)
		{
			//[1] Create timestep node
			TimestepNode *node = new TimestepNode;
			node->osc = m_mesh->m_mesh[i];
			node->timestep = (m_mesh->m_mesh[i]->m_currentTime - m_mesh->m_mesh[i]->m_previousTime);

			//[2] Push the node into the heap and remember the handle
			currentHeap = false;
			handle = heap0.push(node);
		}
		//[3] Push the handle to the timestep on the heap into the "Osc to node" dictionary
		oscillatorToTimestepHandle nd;
		nd.osc = m_mesh->m_mesh[i];
		nd.timestepHandle = handle;
		nd.inHeap = false;
		m_oscillatorTimestepDictionary.push_back(nd);

	}
	//[4] Assign pointers to heaps
	timeStepHeap = &heap0;
	otherHeap = &heap1;
}