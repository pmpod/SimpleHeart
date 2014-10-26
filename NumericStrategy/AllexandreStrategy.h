#pragma once
#include "NumericStrategy\NumericStrategy.h"
#include "NumericStrategy\TimestepNode.h"

//struct timestepNodesss
//{
//	double timestep;
//	Oscillator* osc;
//	vector<timestepNodesss*> neighNodes;
//
//};


class AllexandreStrategy :public NumericStrategy
{
public:
	AllexandreStrategy(CardiacMesh* oscillators);
	~AllexandreStrategy();

	double stepModifiedBackwardEuler(Oscillator * osc);

	double nextStep();
	void reset();

	void constructUpdateTimeTree();
	void constructTimestepTree();
	double guardCellUpdate();
	double synchronise();

	double getDeltaTimestepInc( double& v_prim);
	double getMaxTimestep_Method2b(const double& v_prim, const double& fast_prim, const double& Kd);

private:
	double deltaTimestepIncMean;
	double Kguard;
	double kappaAccuracy;
	double guardUpdateTimestep;

	double lastGuardUpdate;
	bool stimulationStateUpdate;

	TimeTree* m_updateTimeTree;
	vector<oscillatorToUpdateNode> m_oscillatorUpdateDictionary;


	stpHeap heap0;
	stpHeap heap1;
	stpHeap* timeStepHeap;
	stpHeap* otherHeap;

	vector<oscillatorToTimestepHandle> m_oscillatorTimestepDictionary;
	bool currentHeap;

	//calculation pool:
	double v_prim;
	double latestTime;
	double nextTimestep; 
	double bestLocalTimestep;
	double deltaTdiff;
};

