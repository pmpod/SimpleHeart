#pragma once
#include<vector>
#include "Model\Oscillator.h"
#include "Model\CardiacMesh.h"
enum ES_MODE
{
	ES_FREE,
	ES_FIXEDSINGLE,
	ES_FIXEDLOOP,
	ES_SENSING
};
class EpStimulator
{
public:
	EpStimulator();
	~EpStimulator();



	void setStimulationSiteID(int oscid);
	void loadProtocol();
	void saveProtocol();
	int stimulationSiteID();


	void setProtocol(double zeroTime);
	void run(CardiacMesh* mesh);
	void stop(CardiacMesh* mesh);


private:

	void startStimulation(Oscillator* osc, const int& id, CardiacMesh* mesh);
	void stopStimulation(CardiacMesh* mesh);


	double cycleLength_S1;
	int numberOfCycles_S1;
	double cycleLength_S2;
	int numberOfCycles_S2;
	double cycleLength_S3;
	int numberOfCycles_S3;

	double couplingInterval;



	int _stimulationSiteID;
	int _sensingSiteID;

	double _stimulationAmplitude;
	double _stimulationWidth;
	double _stimulationRadius;

	bool _couplingIntervalOn;
	bool _S1On;
	bool _S2On;
	bool _S3On;

	double _sensedActivationTime;
	int _mainCycleLength;


	int _stimulatorMode;



	double lastSpikeTime;




	std::vector<double> stimulationTimes;
};

