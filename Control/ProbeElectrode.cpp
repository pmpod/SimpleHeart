#include "ProbeElectrode.h"

//------------------------------------------------------------
ProbeElectrode::ProbeElectrode()
{
	reset();
	_osc = nullptr;
}

ProbeElectrode::ProbeElectrode(Oscillator* osc)
{
	reset();
	_osc = osc;
}
ProbeElectrode::~ProbeElectrode(void)
{
}

void ProbeElectrode::reset()
{
	intervalsSaved.clear();
	intervalsRR.clear();
	intervalsPotential.clear();
	rangeOffocus = 5;
	lastMaxRR = -1;
	_currentMaxRR = -1;
	maxRRindex = -1;
	refractoryPeriod = 50; //ms
	potentialTreshold = 0.3; //mV
}
void ProbeElectrode::setOscillator(Oscillator*osc)
{
	_osc = osc;
}

void ProbeElectrode::processNewTime(double time)
{
	//[0] Read the Transmembrane potential value from the cell
	double potential = _osc->m_v_scaledPotential;
	//[1] Emit the state calculated signal
	//emit _osc->stateCalculated();
	
	emit newElectrogramAndTime(_osc->m_currentTime, _osc->m_v_electrogram);
	//[1] Simple peak (depolarisation) detection algorithm for transmembrane potential;
	intervalsRR.push_back(time);
	intervalsPotential.push_back(potential);
	if (intervalsRR.size() > 2 * rangeOffocus)
	{
		intervalsRR.pop_front();
		intervalsPotential.pop_front();
	}
	if (potential>potentialTreshold)
	{
		int itSize = intervalsPotential.size();
		for (int i = 1; i<itSize; ++i)
		{
			if (intervalsPotential[i] >= intervalsPotential[i - 1] * (1 + 0.001))
			{
				maxRRindex = i;
				_currentMaxRR = intervalsRR[i];
			}
		}
		if (maxRRindex == rangeOffocus && _currentMaxRR != -1 && (_currentMaxRR > 0) && (_currentMaxRR - lastMaxRR > refractoryPeriod))
		{
			double outer = _currentMaxRR - lastMaxRR;
			emit nextRR(outer);
			intervalsSaved.push_back(outer);
			lastMaxRR = _currentMaxRR;
			_lastRR = outer;
			//_osc->m_lastActivationTime = outer;
		}
		emit curIndex(maxRRindex);
		_currentMaxRR = -1;
	}
}
//---------------------------------------------------------------
double ProbeElectrode::getPositionX()  { return _osc->getPositionX(); }
//---------------------------------------------------------------
double ProbeElectrode::getPositionY(){ return _osc->getPositionY(); }
//---------------------------------------------------------------
double ProbeElectrode::getPositionZ() { return _osc->getPositionZ(); }
double ProbeElectrode::getOscillatorID() { return _osc->oscillatorID; }
//------------------------
double ProbeElectrode::lastRR()
{
	return _lastRR;
}
double ProbeElectrode::currentMaxRR()
{
	return lastMaxRR;
}