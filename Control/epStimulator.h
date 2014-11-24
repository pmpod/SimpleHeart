#pragma once
#include<vector>
#include <QObject>
#include "Model\Oscillator.h"
#include "Model\CardiacMesh.h"
enum ES_MODE
{
	ES_FREE,
	ES_FIXEDLOOP,
	ES_SENSING
};
class EpStimulator : public QObject
{
	Q_OBJECT
protected:
	EpStimulator();
public:
	~EpStimulator();

	static EpStimulator* Instance();


	void setStimulationSiteID(int oscid);
	void loadProtocol();
	void saveProtocol();
	int stimulationSiteID();
	bool stimulatorIsOn();


	void setProtocol(double zeroTime);
	void run(CardiacMesh* mesh, double time);
	void stop(CardiacMesh* mesh);
	void start(CardiacMesh* mesh);
	void stimulatorOn();
	void stimulatorOff();

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

	void setModeFree(bool val);
	void setModeFixedLoop(bool val);
	void setModeSensing(bool val);


private:
	static EpStimulator* _instance;

	void startStimulation(Oscillator* osc, const int& id, CardiacMesh* mesh);
	void stopStimulation(CardiacMesh* mesh);

	double _cycleLength_S1;
	int _numberOfCycles_S1;
	double _cycleLength_S2;
	int _numberOfCycles_S2;
	double _cycleLength_S3;
	int _numberOfCycles_S3;

	double _couplingInterval;



	bool _stimulatorOn;
	int _stimulationSiteID;
	int _sensingSiteID;

	double _stimulationAmplitude;
	double _stimulationWidth;
	double _stimulationRadius;

	bool _S2On;
	bool _S3On;

	double _sensedActivationTime;
	int _mainCycleLength;


	int _stimulatorMode;



	double lastSpikeTime;




	std::vector<double> stimulationTimes;
};

