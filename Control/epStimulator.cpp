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


	_S2On = false;
	_S3On = false;

	_cycleLength_S1 = 500;
	_numberOfCycles_S1 = 5;
	_cycleLength_S2 = 250;
	_numberOfCycles_S2  = 2;
	_cycleLength_S3 = 125;
	_numberOfCycles_S3 = 1;

	_couplingInterval = 200;
	_activationTimeCycle = 200;
	_minRefractoryTreshold = 50.0; //[ms]
	_derivativeTreshold = 0.1;
	_phaseZero = 0;

	_mainCycleLength = 1;

	_activationTimeMode = ATC_RELATIVE;

	_probe.push_back(new ProbeElectrode());
	_probe.push_back(new ProbeElectrode());
	_probe.push_back(new ProbeElectrode());

	_stimulationProtocolBeganOn = 0;
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

ProbeElectrode* EpStimulator::probeElectrode(int num)
{
	return _probe[num];
}
int EpStimulator::probeElectrodeID(int num)
{
	if (num < 0 || num > _probe.size()){
		return -1;
	}
	else {
		return _probe[num]->getOscillatorID();
	}

}
void EpStimulator::setProbeElectrode(CardiacMesh* mesh, int num, int oscID)
{
	if (num < 0 || num > _probe.size()){
	}
	else {
		_probe[num]->setOscillator(mesh->m_mesh[oscID]);
	}
}
int EpStimulator::isProperProbe(int oscID)
{
	for (int i = 0; i < _probe.size(); ++i)
	{
		if (_probe[i]->getOscillatorID() == oscID)
		{
			return i;
		}
	}
	return -1;
}
int EpStimulator::probeElectrodesCount()
{
	return _probe.size();
}

void EpStimulator::run(CardiacMesh* mesh, double time)
{
	//[1] Check the mode
	//[2] On stimulator on Set the proper stimulation times vector / check if they are set
	//[3] Start the protocol
	//[4] If last stimulation was made -stimulator Off
	for (int i = 0; i < _probe.size(); ++i)
	{
		_probe[i]->processNewTime(time);
	}

	if (_stimulatorOn)
	{
		switch (_stimulatorMode)
		{
		case ES_FREE:
			if (mesh->m_mesh[_stimulationSiteID]->getCurrentTime() >= stimulationTimes.back() + _stimulationWidth && mesh->stimulationBegun == true) {
				stopStimulation(mesh);
			}
			break;
		case ES_FIXEDLOOP:
			emit progressOfStimulation(floor(101 * abs(stimulationTimes.front() - time) / abs(stimulationTimes.front() - _stimulationProtocolBeganOn)));
			emit progressOfSinglePace(floor(101 * abs(stimulationTimes.back() - time) / abs(stimulationTimes.back() - _lastStimulationTime)));
			if (stimulationTimes.size() >= 1)
			{
				if (mesh->m_mesh[_stimulationSiteID]->getCurrentTime() >= stimulationTimes.back() && mesh->stimulationBegun == false) {
					startStimulation(mesh->m_mesh[_stimulationSiteID], _stimulationSiteID, mesh);
					_lastStimulationTime = stimulationTimes.back();

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
			//TODO set probe electrode
			if (_sensedActivationTime < 0)
			{
				if (_probe[0]->currentMaxRR() > _stimulationProtocolBeganOn)
				{
					_sensedActivationTime = _probe[0]->currentMaxRR() + _couplingInterval;
					_stimulationProtocolBeganOn = _probe[0]->currentMaxRR();
					setProtocol(_sensedActivationTime);
				}
			}
			else
			{
				emit progressOfStimulation(floor(101 * abs(stimulationTimes.front() - time) / abs(stimulationTimes.front() - _stimulationProtocolBeganOn)));

				emit progressOfSinglePace(floor(101 * abs(stimulationTimes.back() - time) / abs(stimulationTimes.back() - _lastStimulationTime)));

				if (stimulationTimes.size() >= 1)
				{
					if (mesh->m_mesh[_stimulationSiteID]->getCurrentTime() >= stimulationTimes.back() && mesh->stimulationBegun == false) {
						startStimulation(mesh->m_mesh[_stimulationSiteID], _stimulationSiteID, mesh);
						_lastStimulationTime = stimulationTimes.back();
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
			}
			break;
		}
	}
}
void EpStimulator::start(CardiacMesh* mesh)
{
	switch (_stimulatorMode)
	{
	case ES_FREE:
		if (mesh->stimulationBegun == false)
		{
			_stimulatorOn = true;
			startStimulation(mesh->m_mesh[_stimulationSiteID], _stimulationSiteID, mesh);
			stimulationTimes.clear();
			stimulationTimes.push_back(mesh->m_mesh[_stimulationSiteID]->getCurrentTime());
		}
		break;
	case ES_FIXEDLOOP:
		_stimulatorOn = true;
		setProtocol(mesh->m_mesh[_stimulationSiteID]->getCurrentTime());
		_stimulationProtocolBeganOn = mesh->m_mesh[_stimulationSiteID]->getCurrentTime();
		emit stimulatorState(true);
		break;
	case ES_SENSING:
		_stimulatorOn = true;
		_sensedActivationTime = -1;
		_stimulationProtocolBeganOn = mesh->m_mesh[_stimulationSiteID]->getCurrentTime();
		emit stimulatorState(true);
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
	emit stimulatorState(false);
	emit progressOfStimulation(0);
	 
} 
void EpStimulator::setProtocol(double zeroTime) 
{ 
	stimulationTimes.clear();

	double currentTime = zeroTime; 
	 
	int numberOfMainCycles = 1;
	(_stimulatorMode == ES_FIXEDLOOP) ? numberOfMainCycles = _mainCycleLength : numberOfMainCycles = 1;

	for (int j = 1; j <= numberOfMainCycles; ++j)
	{

		stimulationTimes.push_back(currentTime);
		for (int k = 1; k <= _numberOfCycles_S1; ++k) {
			currentTime += _cycleLength_S1;
			stimulationTimes.push_back(currentTime);
		}
		if (_S2On) {
			for (int k = 1; k <= _numberOfCycles_S2; ++k) {

				currentTime += _cycleLength_S2;
				stimulationTimes.push_back(currentTime);
			}
		}
		if (_S3On) {
			for (int k = 1; k <= _numberOfCycles_S3; ++k) {
				currentTime += _cycleLength_S3;
				stimulationTimes.push_back(currentTime);
			}
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


void EpStimulator::processActivationTimes(CardiacMesh* mesh, int oscillatorID)
{
	//[0] Read the Transmembrane potential value from the cell
	Oscillator* osc = mesh->m_mesh[oscillatorID];

	double derivative = (osc->m_v_scaledPotential - osc->m_potentialHistory[0]);

	//[1] Simple peak (depolarisation) detection algorithm for transmembrane potential;

	if (derivative > _derivativeTreshold &&
		(osc->getCurrentTime() - osc->m_lastActivationTime) > _minRefractoryTreshold)
	{
		osc->m_lastActivationTime = osc->getCurrentTime();
		osc->m_lastActivationPhase = fmod((osc->getCurrentTime() - _phaseZero), _activationTimeCycle);
	}
	osc->m_potentialHistory[0] = osc->m_v_scaledPotential;

}
void EpStimulator::setPhaseZero(double val){ _phaseZero = val; }
void EpStimulator::setActivationTimeCycle(double val){ _activationTimeCycle = val; }
ATC_MODE EpStimulator::activationTimeMode(){ return _activationTimeMode; }
double EpStimulator::phaseZero(){ return _phaseZero; }
double EpStimulator::activationTimeCycle(){ return _activationTimeCycle; }
void EpStimulator::setActivationTimeRelative(bool val)
{
	if (val)
	{
		_activationTimeMode = ATC_RELATIVE;
	}

}
void EpStimulator::setActivationTimeFixed(bool val)
{
	if (val)
	{
		_activationTimeMode = ATC_FIXED;
	}

}
void EpStimulator::setActivationTimeSynchronisedS1(bool val)
{
	if (val)
	{
		_activationTimeMode = ATC_S1;
	}

}
void EpStimulator::setActivationTimeSynchronisedLastTCL(bool val)
{
	if (val)
	{
		_activationTimeMode = ATC_LASTTCL;
	}
}
void  EpStimulator::calculateActivationTimeOn(bool val)
{
	_calculateActivationTime = val;
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