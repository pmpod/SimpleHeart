#include "Control\epStimulator.h"
#include<algorithm>

#define NONSET_VAL -999696

EpStimulator* EpStimulator::_instance = nullptr;
EpStimulator::EpStimulator()
{
	_stimulationAmplitude = 1.0;
	_stimulationWidth= 2;
	_stimulationRadius = 0.5;
	_stimulatorMode = ES_FREE;
	_stimulationSiteID = 2200;
	_stimulationSite2ID = 4200;
	_stimulationElectrodeNumber = 0;
	_stimulatorOn = false;
	_measurePPIR = false;

	_S2On = false;
	_S3On = false;

	_cycleLength_S1 = 500;
	_numberOfCycles_S1 = 5;
	_cycleLength_S2 = 250;
	_numberOfCycles_S2  = 2;
	_cycleLength_S3 = 125;
	_numberOfCycles_S3 = 1;

	_measurePPIR = 0;
	_PPIRmeasurementStart = 50;//ms
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

	displayGraphValue = 1;
	for (int i = 0; i < _probe.size(); ++i)
	{
		_probe[i]->setDisplayValue(displayGraphValue);
	}
	_stimulationProtocolBeganOn = 0;
	_lastStimulationInterval = 0;
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

void EpStimulator::setDisplayElectrogramOnGraph(bool b)
{
	displayGraphValue = 1;
	for (int i = 0; i < _probe.size(); ++i)
	{
		_probe[i]->setDisplayValue(displayGraphValue);
	}
}
void EpStimulator::setDisplayPotentialOnGraph(bool b)
{
	displayGraphValue = 2;
	for (int i = 0; i < _probe.size(); ++i)
	{
		_probe[i]->setDisplayValue(displayGraphValue);
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

//---------------------------------------------------------------------------
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
		if (_measurePPIR && _stimulatorMode != ES_FREE)
		{
			int meshSize = mesh->m_mesh.size();
			double minPPIRTCL = -0.95*abs(_referenceTCLforPPIR - _lastStimulationInterval);
			bool temp = false;
			if (time > _stimulationProtocolEndsOn - _PPIRmeasurementStart)
			{
				temp = true;
				for (int j = 0; j < meshSize; ++j)
				{
					if (mesh->m_mesh[j]->m_type != SOLID_WALL)
					{
						mesh->processActivationTime(mesh->m_mesh[j]);
						if (mesh->m_mesh[j]->m_beforeLastActivationTime >= _stimulationProtocolEndsOn && mesh->m_mesh[j]->m_lastPPIR_TCL <= minPPIRTCL)
						{
							mesh->m_mesh[j]->m_lastPPIR_TCL = mesh->m_mesh[j]->m_lastActivationTime - mesh->m_mesh[j]->m_beforeLastActivationTime;
							mesh->m_mesh[j]->m_lastPPIR_TCL -= _referenceTCLforPPIR;
						}
						if (mesh->m_mesh[j]->m_lastPPIR_TCL == NONSET_VAL || mesh->m_mesh[j]->m_lastPPIR_TCL <= minPPIRTCL)
						{
							temp = false;
						}
					}
				}
			}
			if (temp)
			{
				stop(mesh);
				//[1]Stop simulation
				emit stopSimulating();
				//[2]Display PPIR map
				emit displayPPIRTCL(true);
			}

		}
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
				emit progressOfStimulation(floor(100 * abs(stimulationTimes.front() - time) / abs(stimulationTimes.front() - _stimulationProtocolBeganOn)));
				emit progressOfSinglePace(floor(100 * abs(stimulationTimes.back() - time) / abs(stimulationTimes.back() - _lastStimulationTime)));
				if (mesh->m_mesh[_stimulationSiteID]->getCurrentTime() >= stimulationTimes.back() && mesh->stimulationBegun == false) {
					startStimulation(mesh->m_mesh[_stimulationSiteID], _stimulationSiteID, mesh);
					startStimulation(mesh->m_mesh[_stimulationSite2ID], _stimulationSite2ID, mesh);
					//_lastStimulationTime = stimulationTimes.back();
					_lastStimulationTime = mesh->m_mesh[_stimulationSiteID]->getCurrentTime();
				}
				//if (mesh->m_mesh[_stimulationSiteID]->getCurrentTime() >= stimulationTimes.back() + _stimulationWidth && mesh->stimulationBegun == true) {
				if (mesh->m_mesh[_stimulationSiteID]->getCurrentTime() >= _lastStimulationTime + _stimulationWidth && mesh->stimulationBegun == true) {
					stopStimulation(mesh);
					stimulationTimes.pop_back();
				}
			}
			else if (!_measurePPIR){
				stop(mesh);
			}
			break;
		case ES_SENSING:
			//TODO set probe electrode
			if (_sensedActivationTime < 0)
			{
				if (_probe[_stimulationElectrodeNumber]->currentMaxRR() > _stimulationProtocolBeganOn)
				{
					_sensedActivationTime = _probe[_stimulationElectrodeNumber]->currentMaxRR();
					_stimulationProtocolBeganOn = _probe[_stimulationElectrodeNumber]->currentMaxRR() + _couplingInterval;
					setProtocol(_stimulationProtocolBeganOn );
				}
			} 
			else if (stimulationTimes.size() >= 1) {
				emit progressOfStimulation(floor(100 * abs(stimulationTimes.front() - time) / abs(stimulationTimes.front() - _stimulationProtocolBeganOn)));
				emit progressOfSinglePace(floor(100 * abs(stimulationTimes.back() - time) / abs(stimulationTimes.back() - _lastStimulationTime)));
				if (mesh->m_mesh[_stimulationSiteID]->getCurrentTime() >= stimulationTimes.back() && mesh->stimulationBegun == false) {
					startStimulation(mesh->m_mesh[_stimulationSiteID], _stimulationSiteID, mesh);
					_lastStimulationTime = stimulationTimes.back();
				}
				if (mesh->m_mesh[_stimulationSiteID]->getCurrentTime() >= stimulationTimes.back() + _stimulationWidth && mesh->stimulationBegun == true) {
					stopStimulation(mesh);
					stimulationTimes.pop_back();
				}
			}
			else if (!_measurePPIR){
				stop(mesh);
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
		_stimulationProtocolBeganOn = mesh->m_mesh[_stimulationSiteID]->getCurrentTime();
		setProtocol(_stimulationProtocolBeganOn);
		emit stimulatorState(true);
		break;
	case ES_SENSING:
		_stimulatorOn = true;
		_sensedActivationTime = -1;
		_stimulationProtocolBeganOn = mesh->m_mesh[_stimulationSiteID]->getCurrentTime();
		emit stimulatorState(true);
		break;
	}
	//Get last TCL for PPIR measurement
	if ( _measurePPIR && _stimulatorMode != ES_FREE){
		_referenceTCLforPPIR = _probe[_stimulationElectrodeNumber]->lastRR();
		int meshSize = mesh->m_mesh.size();

		for (int j = 0; j < meshSize; ++j)
		{
			mesh->m_mesh[j]->m_lastPPIR_TCL = NONSET_VAL;
		}
	}
}
void EpStimulator::stop(CardiacMesh* mesh)
{
	//switch (_stimulatorMode)
	//{
	//case ES_FREE:
	//	if (mesh->stimulationBegun == true)
	//	{
	//		stopStimulation(mesh);
	//	}
	//	break;
	//}

	stopStimulation(mesh);
	_stimulatorOn = false;
	stimulationTimes.clear();
	_stimulationProtocolBeganOn = 0;
	_lastStimulationInterval = 0;
	_stimulationProtocolEndsOn = DBL_MAX;
	emit stimulatorState(false);
	emit progressOfStimulation(0);
	emit progressOfSinglePace(0);
} 
void EpStimulator::setProtocol(double zeroTime) 
{ 
	stimulationTimes.clear();

	double currentTime = zeroTime; 
	_stimulationSiteID = _probe[_stimulationElectrodeNumber]->getOscillatorID();
	_stimulationSiteID = _probe[0]->getOscillatorID();//XXXblockTest
	_stimulationSite2ID = _probe[1]->getOscillatorID();//XXXblockTest
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
	_lastStimulationInterval = stimulationTimes[0] - stimulationTimes[1];
	_stimulationProtocolEndsOn = stimulationTimes.front();
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
//---------------------------------------------------------------------------
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
void EpStimulator::setStimulationElectrodeNumber(int elID) { _stimulationElectrodeNumber = elID; }

void EpStimulator::setMeasurePPIR(bool b){ _measurePPIR = b; }
void EpStimulator::setCouplingInterval(int val){ _couplingInterval = val; }
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

	double derivative = (osc->m_v_scaledPotential - osc->m_potentialHistory);

	//[1] Simple peak (depolarisation) detection algorithm for transmembrane potential;

	if (derivative > _derivativeTreshold &&
		(osc->getCurrentTime() - osc->m_lastActivationTime) > _minRefractoryTreshold)
	{
		osc->m_beforeLastActivationTime = osc->m_lastActivationTime;
		osc->m_lastActivationTime = osc->getCurrentTime();
	}
	osc->m_potentialHistory = osc->m_v_scaledPotential;
}

void EpStimulator::setPhaseZero(double val){ _phaseZero = val; }
void EpStimulator::setActivationTimeCycle(double val){ _activationTimeCycle = val; }
ATC_MODE EpStimulator::activationTimeMode(){ return _activationTimeMode; }
double EpStimulator::phaseZero(){ return _phaseZero; }

double EpStimulator::referenceTCLforPPIR(){ return _referenceTCLforPPIR; }
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
int  EpStimulator::stimulationElectrodeNumber(){ return _stimulationElectrodeNumber; }
void EpStimulator::setStimulusAmplitude(double val){ _stimulationAmplitude = val; }
void EpStimulator::setStimulusWidth(int val){ _stimulationWidth = val; }
void EpStimulator::setStimulusRadius(double val){ _stimulationRadius = val; }