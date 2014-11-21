#include "Control\epStimulator.h"
#include<algorithm>


EpStimulator::EpStimulator()
{
	_stimulationAmplitude = 0.1;
	_stimulationWidth= 10;
	_stimulationRadius = 2;
	_stimulatorMode = ES_FREE;
}


EpStimulator::~EpStimulator()
{
}

void EpStimulator::run(CardiacMesh* mesh)
{
	switch (_stimulatorMode)
	{
	case ES_FREE:
		if (mesh->stimulationBegun == false)
		{
			startStimulation(mesh->m_mesh[_stimulationSiteID], _stimulationSiteID, mesh);
		}

		//else if (!m_definitions->m_ectopicActivity && stimulationBegun == true)
		//{
		//	stopStimulation();
		//}



	}

}
void EpStimulator::stop(CardiacMesh* mesh)
{
	switch (_stimulatorMode)
	{
	case ES_FREE:
		if (mesh->stimulationBegun == true)
		{
			stopStimulation(mesh);
		}



	}

}
void EpStimulator::setProtocol(double zeroTime)
{
	stimulationTimes.clear();

	double currentTime = zeroTime;

	int numberOfMainCycles = 1;
	(_stimulatorMode == ES_FIXEDLOOP) ? numberOfMainCycles = _mainCycleLength : numberOfMainCycles = 1;
	for (int j = 1; j < numberOfMainCycles; ++j)
	if (_S1On) {
		for (int k = 1; k <= numberOfCycles_S1; ++k) {

			stimulationTimes.push_back(currentTime);
			currentTime += cycleLength_S1;
		}
	}
	if (_S2On) {
		for (int k = 1; k <= numberOfCycles_S2; ++k) {

			stimulationTimes.push_back(currentTime);
			currentTime += cycleLength_S2;
		}
	}
	if (_S3On) {
		for (int k = 1; k <= numberOfCycles_S3; ++k) {

			stimulationTimes.push_back(currentTime);
			currentTime += cycleLength_S3;
		}
	}

	std::reverse(stimulationTimes.begin(), stimulationTimes.end());

	//nextSpikeTime = stimulationTimes.back();
	//stimulationTimes.pop_back();
}
//---------------------------------------------------------------------------
void EpStimulator::startStimulation(Oscillator* osc, const int& id, CardiacMesh* mesh)
{
	if (!osc->m_underStimulation)
	{
		mesh->m_underStimulation.push_back(osc);
		osc->m_underStimulation = true;
		osc->m_stimulation = _stimulationAmplitude;
	}
	double distance;
	for (int k = 0; k < osc->m_neighbours.size(); ++k)
	{
		distance = std::pow((mesh->m_mesh[id]->m_x - osc->m_neighbours[k]->m_x), 2) +
			std::pow((mesh->m_mesh[id]->m_y - osc->m_neighbours[k]->m_y), 2) +
			std::pow((mesh->m_mesh[id]->m_z - osc->m_neighbours[k]->m_z), 2);
		if (distance <= _stimulationRadius && !(osc->m_neighbours[k]->m_underStimulation))
		{
			startStimulation(osc->m_neighbours[k], id, mesh);
		}
	}
	mesh->stimulationBegun = true;
}
void EpStimulator::stopStimulation(CardiacMesh* mesh)
{
	for (int j = 0; j < mesh->m_underStimulation.size(); ++j)
	{
		mesh->m_underStimulation[j]->m_underStimulation = false;
		mesh->m_underStimulation[j]->m_stimulation = 0.0;
	}
	mesh->m_underStimulation.clear();
	mesh->stimulationBegun = false;
}
//
void EpStimulator::setStimulationSiteID(int oscid)
{
	_stimulationSiteID = oscid;
}

//burster::burster()
//{
//	lastSpike = 0;
//	currentPhaseOfPeak = 0;
//	currentPhase = 0;
//	meanPeriod = 70;
//	periodModulation = 0;
//	amp = -0.1;
//	length = 1;
//}
//double burster::spike(double time, double value)
//{
//	double retValue;
//	currentPhase = time - lastSpike;
//	if (currentPhaseOfPeak == 0)
//	{
//		periodModulation = value;
//		if (meanPeriod + periodModulation <= length + 1)
//		{
//			periodModulation = length + 1 - meanPeriod;
//		}
//		lastSpike = time;
//		retValue = amp;
//		currentPhaseOfPeak++;
//	}
//	else if (currentPhaseOfPeak < length)
//	{
//		retValue = amp;
//		currentPhaseOfPeak++;
//	}
//	else
//	{
//		retValue = 0;
//	}
//
//	if (currentPhase >= meanPeriod + periodModulation)
//	{
//		currentPhaseOfPeak = 0;
//		lastSpike = time;
//	}
//	return retValue;
//}