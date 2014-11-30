#pragma once
#include<vector>
#include<deque>
#include <QObject>
#include "Model\Oscillator.h"
#include "Model\CardiacMesh.h"
#include "Control\ProbeElectrode.h"
enum ES_MODE
{
	ES_FREE,
	ES_FIXEDLOOP,
	ES_SENSING
};


enum ATC_MODE{
	ATC_RELATIVE,
	ATC_FIXED,
	ATC_LASTTCL,
	ATC_S1,

};

class EpStimulator : public QObject
{
	Q_OBJECT
protected:
	EpStimulator();
public:
	~EpStimulator();

	static EpStimulator* Instance();

	void loadProtocol();
	void saveProtocol();


	void setProtocol(double zeroTime);
	void run(CardiacMesh* mesh, double time);
	void stop(CardiacMesh* mesh);
	void start(CardiacMesh* mesh);

	//Setters:
	void stimulatorOn();
	void stimulatorOff();
	void setStimulationSiteID(int oscid);

	//Getters:
	bool stimulatorIsOn();
	int stimulationSiteID();
	double phaseZero();
	double activationTimeCycle();
	bool measurePPIR();
	double referenceTCLforPPIR();
	int stimulationElectrodeNumber();
	void processActivationTimes(CardiacMesh* mesh,  int oscillatorID);
	ATC_MODE activationTimeMode();
public slots:
	void setCycleLength_S1(int val);
	void setCycleLength_S2(int val);
	void setCycleLength_S3(int val);
	void setNumberOfCycles_S1(int val);
	void setNumberOfCycles_S2(int val);
	void setNumberOfCycles_S3(int val);
	void setS2On(bool val);
	void setS3On(bool val);
	void setCouplingInterval(int val);

	void setStimulusAmplitude(double val);
	void setStimulusWidth(int val);
	void setStimulusRadius(double val);
	void setStimulationElectrodeNumber(int elID);

	void setModeFree(bool val);
	void setModeFixedLoop(bool val);
	void setModeSensing(bool val);
	int probeElectrodeID(int num);
	void setProbeElectrode(CardiacMesh* mesh, int num, int oscID);
	ProbeElectrode* probeElectrode(int num);
	int isProperProbe(int oscID);
	int probeElectrodesCount();


	void setPhaseZero(double val);
	void setActivationTimeCycle(double val);

	void setMeasurePPIR(bool b);
	void setActivationTimeRelative(bool val);
	void setActivationTimeFixed(bool val);
	void setActivationTimeSynchronisedS1(bool val);
	void setActivationTimeSynchronisedLastTCL(bool val);
	void calculateActivationTimeOn(bool val);
signals:
	void progressOfStimulation(int);
	void progressOfSinglePace(int);
	void currentActivationTimeCycle(int);
	void currentActivationTimePhase(int);
	void stimulatorState(bool);
	void stopSimulating();
	void displayPPIRTCL(bool);

private:
	static EpStimulator* _instance;

	void startStimulation(Oscillator* osc, const int& id, CardiacMesh* mesh);
	void stopStimulation(CardiacMesh* mesh);

	// Stimulation settings field
	//---------------------------------------------------
	int _stimulatorMode;

	int _mainCycleLength;
	double _cycleLength_S1;
	int _numberOfCycles_S1;
	double _cycleLength_S2;
	int _numberOfCycles_S2;
	double _cycleLength_S3;
	int _numberOfCycles_S3;

	double _couplingInterval;
	double _sensedActivationTime;

	double _stimulationProtocolBeganOn;
	double _stimulationProtocolEndsOn;
	double _lastStimulationTime;
	double _lastStimulationInterval;
	
	bool _stimulatorOn;
	int _stimulationSiteID;
	int _stimulationElectrodeNumber;
	int _sensingSiteID;

	double _stimulationAmplitude;
	double _stimulationWidth;
	double _stimulationRadius;

	bool _S2On;
	bool _S3On;
	bool _measurePPIR;
	double _referenceTCLforPPIR; 
	double _PPIRmeasurementStart;
	std::deque<double> stimulationTimes;
	std::vector<ProbeElectrode*>	_probe;


	//Activation Time Display
	//---------------------------------------------------
	double _derivativeTreshold;
	double _minRefractoryTreshold;
	double _phaseZero;
	double _activationTimeCycle;
	ATC_MODE _activationTimeMode;
	bool _calculateActivationTime;

};

