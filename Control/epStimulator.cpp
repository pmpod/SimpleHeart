#include "Control\epStimulator.h"
#include<algorithm>

EpStimulator* EpStimulator::_instance = nullptr;
EpStimulator::EpStimulator()
{
	_stimulationAmplitude = 0.1;
	_stimulationWidth= 10;
	_stimulationRadius = 2;
	_stimulatorMode = ES_FREE;
	_stimulationSiteID = 2200;
	_stimulatorOn = false;


	_S2On = true;
	_S3On = true;

	_cycleLength_S1 = 500;
	_numberOfCycles_S1 = 5;
	_cycleLength_S2 = 250;
	_numberOfCycles_S2  = 2;
	_cycleLength_S3 = 125;
	_numberOfCycles_S3 = 1;

	_mainCycleLength = 1;
}


EpStimulator::~EpStimulator()
{
}

EpStimulator* EpStimulator::Instance()
{
	if (EpStimulator::_instance == nullptr)
	{
		_instance = new EpStimulator();
	}

	return _instance;
}

bool EpStimulator::stimulatorIsOn()
{
	return _stimulatorOn;
}

void EpStimulator::run(CardiacMesh* mesh, double time)
{
	//[1] Check the mode
	//[2] On stimulator on Set the proper stimulation times vector / check if they are set
	//[3] Start the protocol
	//[4] If last stimulation was made -stimulator Off
	switch (_stimulatorMode)
	{
	case ES_FREE:
		if (mesh->m_mesh[_stimulationSiteID]->getCurrentTime() >= stimulationTimes.back() + _stimulationWidth && mesh->stimulationBegun == true) {
			stopStimulation(mesh);
		}
		break;
	case ES_FIXEDLOOP:
		if (stimulationTimes.size() >= 1)
		{
			if (mesh->m_mesh[_stimulationSiteID]->getCurrentTime() >= stimulationTimes.back() && mesh->stimulationBegun == false) {
				startStimulation(mesh->m_mesh[_stimulationSiteID], _stimulationSiteID, mesh);
			}
			if (mesh->m_mesh[_stimulationSiteID]->getCurrentTime() >= stimulationTimes.back() + _stimulationWidth && mesh->stimulationBegun == true) {
				stopStimulation(mesh);
				stimulationTimes.pop_back();
			}
		}
		else
		{
			stop(mesh);
		}
		break;
	case ES_SENSING:
		break;
	}
}


void EpStimulator::start(CardiacMesh* mesh)
{
	switch (_stimulatorMode)
	{
	case ES_FREE:
		if (mesh->stimulationBegun == false)
		{
			startStimulation(mesh->m_mesh[_stimulationSiteID], _stimulationSiteID, mesh);
			stimulationTimes.clear();
			stimulationTimes.push_back(mesh->m_mesh[_stimulationSiteID]->getCurrentTime());
			_stimulatorOn = true;
		}
		break;
	case ES_FIXEDLOOP:
		setProtocol(mesh->m_mesh[_stimulationSiteID]->getCurrentTime());
		_stimulatorOn = true;
		break;
	case ES_SENSING:
		break;
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
		break;
	}
	stopStimulation(mesh);
	_stimulatorOn = false;
	 
} 
void EpStimulator::setProtocol(double zeroTime) 
{ 
	stimulationTimes.clear();

	double currentTime = zeroTime; 
	 
	int numberOfMainCycles = 1;
	(_stimulatorMode == ES_FIXEDLOOP) ? numberOfMainCycles = _mainCycleLength : numberOfMainCycles = 1;
	for (int j = 1; j <= numberOfMainCycles; ++j)
	{
		
		for (int k = 1; k <= _numberOfCycles_S1; ++k) {
			stimulationTimes.push_back(currentTime);
			currentTime += _cycleLength_S1;
		}
		if (_S2On) {
			for (int k = 1; k <= _numberOfCycles_S2; ++k) {

				stimulationTimes.push_back(currentTime);
				currentTime += _cycleLength_S2;
			}
		}
		if (_S3On) {
			for (int k = 1; k <= _numberOfCycles_S3; ++k) {

				stimulationTimes.push_back(currentTime);
				currentTime += _cycleLength_S3;
			}
		}
		stimulationTimes.push_back(currentTime);
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

void EpStimulator::stimulatorOn() { _stimulatorOn = true; }
void EpStimulator::stimulatorOff(){ _stimulatorOn = false; }

void  EpStimulator::setCycleLength_S1(int val){ _cycleLength_S1 = val; }
void  EpStimulator::setCycleLength_S2(int val){ _cycleLength_S2 = val; }
void  EpStimulator::setCycleLength_S3(int val){ _cycleLength_S3 = val; }
void  EpStimulator::setNumberOfCycles_S1(int val){ _numberOfCycles_S1 = val; }
void  EpStimulator::setNumberOfCycles_S2(int val){ _numberOfCycles_S2 = val; }
void  EpStimulator::setNumberOfCycles_S3(int val){ _numberOfCycles_S3 = val; }
void  EpStimulator::setS2On(bool val){ _S2On = val; }
void  EpStimulator::setS3On(bool val){ _S3On = val; }
void  EpStimulator::setCouplingInterval(int val){ _couplingInterval = val; }
void EpStimulator::setModeFree(bool val)
{
	if (val){
		_stimulatorMode = ES_FREE;
	}
}
void EpStimulator::setModeFixedLoop(bool val)
{
	if (val){
		_stimulatorMode = ES_FIXEDLOOP;
	}
}
void EpStimulator::setModeSensing(bool val)
{
	if (val){
		_stimulatorMode = ES_SENSING;
	}
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