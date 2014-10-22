#include "NumericStrategy\AllexandreStrategy.h"
#include <algorithm>

AllexandreStrategy::AllexandreStrategy(CardiacMesh* oscillators) : NumericStrategy(oscillators)
{
	deltaTimestepIncMean = oscillators->getDeltaR()*oscillators->getDeltaR() / (2 * oscillators->m_maximumCV);
	deltaTdiff = oscillators->getDeltaR() * oscillators->getDeltaR();
	guardUpdateTimestep = oscillators->m_maximumCV / oscillators->getDeltaR();
	kappaAccuracy = 0.1;
	Kguard = 20;

	//Create time trees
	m_updateTimeTree = nullptr;
	constructUpdateTimeTree();

	constructTimestepTree();
	
	reset();
}
//---------------------------------------------------------------------------------
void AllexandreStrategy::reset()
{
	NumericStrategy::reset();
	earliestTime = 0;
	latestTime = 0;
	lastGuardUpdate = -guardUpdateTimestep-1;
	resetTime(m_updateTimeTree, 0.0);
	//Reset timesteptree!
	stimulationStateUpdate = false;
	guardCellUpdate();
}
AllexandreStrategy::~AllexandreStrategy()
{
	clear(m_updateTimeTree);
	timeStepHeap->clear();
	otherHeap->clear();
	m_oscillatorTimestepDictionary.clear();
}
//---------------------------------------------------------------------------------
double AllexandreStrategy::nextStep()
{
	//[0] Check for the stimulation going on
	if (m_mesh->stimulationBegun && !stimulationStateUpdate)
	{
		double Dt;
		double dTime;
		//update potentials in all stimulated cells
		for (unsigned short k = 0; k < m_mesh->m_underStimulation.size(); ++k)
		{
			Oscillator* osc = m_mesh->m_underStimulation[k];
			//bestLocalTimestep = stepModifiedBackwardEuler(osc);
			stepModifiedBackwardEuler(osc);
			v_prim = (osc->getPotential() - osc->getPreviousPotential());
			//bublle time
			bubbleNewTime(m_oscillatorUpdateDictionary[osc->oscillatorID].p_updateTimeTreeNode, osc->getCurrentTime());
			// Update the timestep set

			//Dt = deltaTdiff / ( 5*2 * osc->m_connexin[0]);
			//dTime = 1;
			//dTime = getMaxTimestep_Method2b(v_prim / Dt, osc->m_currentPRIM[0], Dt* (osc->m_connexin[0]));

			dTime = min(deltaTdiff / (2 * osc->m_connexin[0]), (timeStepHeap->top())->timestep);
			//dTime = 1;
			//dTime = getMaxTimestep_Method2b(v_prim / Dt, osc->m_currentPRIM[0], Dt* (osc->m_connexin[0]));

			(*(m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle))->timestep = dTime;
			timeStepHeap->update(m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle);
		}
		guardCellUpdate();
		stimulationStateUpdate = true;
	}
	else if (!m_mesh->stimulationBegun && stimulationStateUpdate)
	{
		stimulationStateUpdate = false;
	}

	//[1] Find earliest node
	TimeTree* currentNode = goToEarliest(m_updateTimeTree);
	earliestTime = currentNode->time;

	if (earliestTime > latestTime)
		latestTime = earliestTime;
	//[2] Update gurad cell algorithm if necessary
	if (latestTime - lastGuardUpdate >= guardUpdateTimestep)
	{
		guardCellUpdate();
		lastGuardUpdate = latestTime;
	}

	//[4] Get the oscillator and calculate new timestep
	Oscillator* osc = currentNode->osc;
	double lastLocalTimestep = osc->getCurrentTime();
	double bestLocalTimestep = stepModifiedBackwardEuler(osc);

	//[5] Update the time update tree
	bubbleNewTime(currentNode, osc->getCurrentTime());

	//[6] Update the timestep set
	(*(m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle))->timestep = bestLocalTimestep;

	//if (lastLocalTimestep > bestLocalTimestep)
	//	timeStepHeap->increase(m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle);
	//else
		timeStepHeap->update(m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle);

	return earliestTime;
}
double AllexandreStrategy::stepModifiedBackwardEuler(Oscillator * osc)
{
	//[1] determine Next MAx Local Timestep Method2b
	nextTimestep = (osc->getCurrentTime() - osc->getPreviousTime());
	v_prim = (osc->getPotential() - osc->getPreviousPotential());
	bestLocalTimestep = getMaxTimestep_Method2b(v_prim / nextTimestep, osc->m_currentPRIM[0], nextTimestep* (osc->m_connexin[0]));

	//[2] Update the timestep set
	//bubbleNewTime(m_oscillatorNodeDictionary[osc->oscillatorID].p_timestepTreeNode, nextTimestep);

	if (bestLocalTimestep > osc->guardTimestep)
		nextTimestep = osc->guardTimestep;
	else
		nextTimestep = bestLocalTimestep;

	//[2] Set new current time on oscillator
	osc->setCurrentTime(osc->getCurrentTime() + nextTimestep);

	//[3] Calculate new step 
	if (osc->getCellType() != SOLID_WALL)
	{
		if (osc->m_underStimulation == false)
		{
			v_prim = osc->m_v_potential + nextTimestep * (osc->getPotentialPrim() + osc->getExtrapolatedNeighbourSource());

			v_prim = v_prim / (1 + nextTimestep * osc->m_ConnexinSum);

			int numberOfEquations = osc->getNumberOfCurrents();
			for (int k = 0; k < numberOfEquations; ++k)
			{
				osc->setCurrent(osc->m_v_current[k] + nextTimestep * (osc->getCurrentPrim(k)), k);
			}
			osc->setPreviousPotential(osc->m_v_potential);
			osc->setPotential(v_prim);
			osc->setElectrogram(osc->m_v_potential);
			//osc->setElectrogram(nextTimestep);
		}
		else
		{
			osc->setPreviousPotential(osc->m_v_potential);
			osc->setPotential(m_mesh->m_ectopicAmplitude);
			osc->setElectrogram(osc->m_v_potential);
			//osc->setElectrogram(nextTimestep);
		}
	}
	return bestLocalTimestep;
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
		v_prim = (osc->getPotential() - osc->getPreviousPotential()) / (osc->getCurrentTime() - osc->getPreviousTime());
		//if (!osc->m_underStimulation)
		//{
		//	updateTime  = node->timestep + getDeltaTimestepInc(v_prim);
		//}
		//else
		//{
		//	Dt = deltaTdiff / (2 * osc->m_connexin[0]);
		//	updateTime = getMaxTimestep_Method2b(v_prim / Dt, osc->m_currentPRIM[0], Dt* (osc->m_connexin[0])) + getDeltaTimestepInc(v_prim);
		//}
		updateTime = node->timestep + getDeltaTimestepInc(v_prim);



		//[3] Update neighbours
		short numberOfNeighs = osc->m_neighbours.size();
		for (short k = 0; k < numberOfNeighs; ++k)
		{
			nosc = osc->m_neighbours[k];
			handle = m_oscillatorTimestepDictionary[nosc->oscillatorID].timestepHandle;

			//Check, if the neighbour is still in the current heap
			if ((m_oscillatorTimestepDictionary[nosc->oscillatorID].inHeap == currentHeap))
			{
				if ((*handle)->timestep > updateTime) //&& notUpdated;
				{
					//[A] mark Updated
					////(*handle)->updated = true;
					//[B] update current timestep in the node
					(*handle)->timestep = updateTime;
					timeStepHeap->increase(handle);
					//[C] update guardTimestep
					nosc->guardTimestep = updateTime;
				}
				else
				{
					nosc->guardTimestep = (*handle)->timestep;
					////(*handle)->updated = true; //XXXX?
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

	//STYMULACJA NAPRAWIC TO!!!!!!!
	double f = kappaAccuracy / ((1 + Kd / 2) * (abs(v_prim) + abs(fast_prim)));
	if (f > 5)
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
	newTreeTrunk(m_updateTimeTree, m_mesh->m_mesh,m_oscillatorUpdateDictionary);

	resetTime(m_updateTimeTree, 0.0);
}
//---------------------------------------------------------------------------------
void AllexandreStrategy::constructTimestepTree()
{
	double initValue = 0.2;
	if (m_mesh->m_mesh.empty())
		return;

	//TimestepNode* node;
	stpHeap::handle_type handle;
	for (int i = 0; i < m_mesh->m_mesh.size(); ++i)
	{
		TimestepNode *node = new TimestepNode;
		//node->updated = false;
		node->osc = m_mesh->m_mesh[i];
		node->timestep = initValue;
		//[1] Store pointers to every timestep node in a vector
		//node = newTimestepNodeP(, );
		//m_timestepNodes.push_back(node);

		//[2] Push the node into the heap and remember the handle
		currentHeap = false;
		handle = heap0.push(node);

		//[3] Push the handle to the timestep on the heap into the Osc-> node dictionary
		oscillatorToTimestepHandle nd;
		nd.osc = m_mesh->m_mesh[i];
		nd.timestepHandle = handle;
		nd.inHeap = false;

		m_oscillatorTimestepDictionary.push_back(nd);
		timeStepHeap = &heap0;
		otherHeap = &heap1;
	}
}
//Oscillator *nosc;
//Oscillator *osc;
////double v_prim = osc->getLastCurrentSource() + osc->getPotentialPrim();

//TimeTree* timestepNode = goToEarliest(m_timestepTree);
//TimeTree* noscNode;
//double val;
////[2] Po kolei zupdateowanie wszystkich deltaT_guard
//while (timestepNode != nullptr)
//{
//	osc = timestepNode->osc;
//	double v_prim = (osc->getPotential() - osc->getPreviousPotential()) / osc->getCurrentTimestep();

//	osc->guardTimestep = timestepNode->time;
//	//osc->guardTimestep = osc->m_currentTimestep;
//	for (int i = 1; i < osc->m_neighbours.size(); ++i)
//	{
//		nosc = osc->m_neighbours[i];
//		noscNode = m_oscillatorNodeDictionary[nosc->oscillatorID].p_timestepTreeNode;
//		nosc->guardTimestep = min(noscNode->time, osc->getCurrentTimestep() + getDeltaTimestepInc(v_prim));
//		nosc->m_currentTimestep = nosc->guardTimestep;

//		bubbleNewTime(noscNode, nosc->guardTimestep);

//		//Update timestepu o t_inc
//		//? 1 : 0)
//		//oznaczenie oscylatora jako zupdateowany
//	}
//	timestepNode = goToGreaterThan(m_timestepTree, osc->guardTimestep);
//}

//
//ouble AllexandreStrategy::guardCellUpdate()
//{
//	TimestepNode* node;
//	Oscillator* osc;
//	Oscillator* nosc;
//	boost::heap::fibonacci_heap<TimestepNode*, boost::heap::compare<compare_TimestepNode>>::handle_type handle;
//	double Dt;
//	double updateTime;
//	int counter = 0;
//	for (typename PriorityQueue::iterator it = begin; it != end; ++it)
//		while (!timeStepHeap->empty())
//		{
//		//[1] Take the top node
//		node = timeStepHeap->top();
//		osc = node->osc;
//
//		//[1] Update the guard Timestep based on the node timestep
//		osc->guardTimestep = node->timestep;
//
//		//[2] Prepare timestep value to update neighbours
//		v_prim = (osc->getPotential() - osc->getPreviousPotential());
//		//if (!osc->m_underStimulation)
//		//{
//		//	updateTime  = node->timestep + getDeltaTimestepInc(v_prim);
//		//}
//		//else
//		//{
//		//	Dt = deltaTdiff / (2 * osc->m_connexin[0]);
//		//	updateTime = getMaxTimestep_Method2b(v_prim / Dt, osc->m_currentPRIM[0], Dt* (osc->m_connexin[0])) + getDeltaTimestepInc(v_prim);
//		//}
//		updateTime = node->timestep + getDeltaTimestepInc(v_prim);
//
//
//
//		//[3] Update neighbours
//		short numberOfNeighs = osc->m_neighbours.size();
//		for (short k = 0; k < numberOfNeighs; ++k)
//		{
//			nosc = osc->m_neighbours[k];
//			handle = m_oscillatorTimestepDictionary[nosc->oscillatorID].timestepHandle;
//
//			//Check, if the neighbour is still in the current heap
//			if ((m_oscillatorTimestepDictionary[nosc->oscillatorID].inHeap == currentHeap))
//			{
//				if ((*handle)->timestep > updateTime) //&& notUpdated;
//				{
//					//[A] mark Updated
//					////(*handle)->updated = true;
//					//[B] update current timestep in the node
//					(*handle)->timestep = updateTime;
//					timeStepHeap->update(handle);
//					//[C] update guardTimestep
//					nosc->guardTimestep = updateTime;
//				}
//				else
//				{
//					nosc->guardTimestep = (*handle)->timestep;
//					////(*handle)->updated = true; //XXXX?
//					timeStepHeap->update(handle);
//				}
//			}
//
//		}
//		//[4] Pop the node from the current heap
//		timeStepHeap->pop();
//		//[5] Insert the node into the new heap & update the dictionary
//		m_oscillatorTimestepDictionary[osc->oscillatorID].inHeap = (!currentHeap);
//		m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle = otherHeap->push(node);
//		}
//	std::swap(timeStepHeap, otherHeap);
//	currentHeap = !currentHeap;
//	return 1;
//}

//double AllexandreStrategy::guardCellUpdate()
//{
//	TimestepNode* node;
//	Oscillator* osc;
//	Oscillator* nosc;
//	boost::heap::fibonacci_heap<TimestepNode*, boost::heap::compare<compare_TimestepNode>>::handle_type handle;
//	boost::heap::fibonacci_heap<TimestepNode*, boost::heap::compare<compare_TimestepNode>>::ordered_iterator iter;
//	int counterMax = m_mesh->m_mesh.size();
//	int counter = 0;
//	double Dt;
//	double updateTime;
//
//	while (counter < counterMax)
//	{
//		iter = timeStepHeap->ordered_begin();
//		std::advance(iter, counter);
//		//iter.advance(counter);
//		//[1] Take the top node
//		node = (*iter);
//		osc = node->osc;
//
//		//[1] Update the guard Timestep based on the node timestep
//		osc->guardTimestep = node->timestep;
//
//		//[2] Prepare timestep value to update neighbours
//		v_prim = (osc->getPotential() - osc->getPreviousPotential());
//		//if (!osc->m_underStimulation)
//		//{
//		//	updateTime  = node->timestep + getDeltaTimestepInc(v_prim);
//		//}
//		//else
//		//{
//		//	Dt = deltaTdiff / (2 * osc->m_connexin[0]);
//		//	updateTime = getMaxTimestep_Method2b(v_prim / Dt, osc->m_currentPRIM[0], Dt* (osc->m_connexin[0])) + getDeltaTimestepInc(v_prim);
//		//}
//		updateTime = node->timestep + getDeltaTimestepInc(v_prim);
//
//
//
//		//[3] Update neighbours
//		short numberOfNeighs = osc->m_neighbours.size();
//		for (short k = 0; k < numberOfNeighs; ++k)
//		{
//			nosc = osc->m_neighbours[k];
//			handle = m_oscillatorTimestepDictionary[nosc->oscillatorID].timestepHandle;
//
//			//Check, if the neighbour is still in the current heap
//			if ((m_oscillatorTimestepDictionary[nosc->oscillatorID].inHeap == currentHeap))
//			{
//				if ((*handle)->timestep > updateTime) //&& notUpdated;
//				{
//					//[A] mark Updated
//					////(*handle)->updated = true;
//					//[B] update current timestep in the node
//					(*handle)->timestep = updateTime;
//					timeStepHeap->update(handle);
//					//[C] update guardTimestep
//					nosc->guardTimestep = updateTime;
//				}
//				else
//				{
//					nosc->guardTimestep = (*handle)->timestep;
//					////(*handle)->updated = true; //XXXX?
//					timeStepHeap->update(handle);
//				}
//			}
//
//		}
//		//[4] Pop the node from the current heap
//		//timeStepHeap->pop();
//		//[5] Insert the node into the new heap & update the dictionary
//		m_oscillatorTimestepDictionary[osc->oscillatorID].inHeap = (!currentHeap);
//		//m_oscillatorTimestepDictionary[osc->oscillatorID].timestepHandle = otherHeap->push(node);
//		++counter;
//	}
//	//std::swap(timeStepHeap, otherHeap);
//	currentHeap = !currentHeap;
//	return 1;