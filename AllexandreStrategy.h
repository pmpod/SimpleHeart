#pragma once
#include "NumericStrategy.h"
#include "TimestepNode.h"

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

	//TimeTree* m_timestepTree;

	//vector<timestepNode*> m_timestepList;
	//vector<timestepNode*> m_timestepDictionary;








	stpHeap heap0;
	stpHeap heap1;
	stpHeap* timeStepHeap;
	stpHeap* otherHeap;
	vector<TimestepNode*> m_timestepNodes;


	vector<oscillatorToTimestepHandle> m_oscillatorTimestepDictionary;
	bool currentHeap;

	//calculation pool:
	double v_prim;
	double earliestTime;
	double latestTime;
	double nextTimestep; 
	double bestLocalTimestep;
	double deltaTdiff;
};

