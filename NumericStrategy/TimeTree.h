#pragma once
#include "Model\Oscillator.h"

struct oscillatorToUpdateNode
{
	Oscillator* osc;
	TimeTree* p_updateTimeTreeNode;
};

struct TimeTree
{
	double time;
	TimeTree* earlierNode;
	TimeTree* laterNode;
	TimeTree* parentNode;
	Oscillator* osc;
};

///Create time binary tree node
TimeTree *newTreeNode(const double&  newUpdateTime = DBL_MAX, TimeTree * parent = nullptr, Oscillator *osc = nullptr);
void clear(TimeTree *&node);

///Create whole empty tree trunk
TimeTree *newTreeTrunk(TimeTree *&node, const int& modelDepth, double initValue = DBL_MAX);
///Create whole empty tree trunk with oscillatora
TimeTree *newTreeTrunk(TimeTree *&node, vector<Oscillator*>& mesh, int& numberOfOscillatorsAdded, vector<oscillatorToUpdateNode>& dictionary, int treeDepth, double initValue = DBL_MAX);
TimeTree *newTreeTrunk(TimeTree *&node, vector<Oscillator*>& mesh, vector<oscillatorToUpdateNode>& dictionary, double initValue = DBL_MAX);
TimeTree *goToEarliest(TimeTree *node);



TimeTree *resetUpdateIndicator(TimeTree *node);
TimeTree *resetTime(TimeTree *node, double val);

void swapChildrenIfNecessary(TimeTree *&node);
void bubbleNewTime(TimeTree *&node, const double& newTime);
void bubbleNewTimeFullCheck(TimeTree *&node, const double& newTime);



TimeTree *goToGreaterThan(TimeTree *node, const double& val);
TimeTree *goToNotUpdatedEarliest(TimeTree *node);
TimeTree* addOscillator(TimeTree *&node, Oscillator* nosc, double initValue = 0);
TimeTree* insertTreeNode(TimeTree *node, double newUpdateTime = 0, Oscillator* nosc = NULL);
//static TimeTree *newTimeTree(const int& modelSize);
//TimeTree *newTreeNode(double newUpdateTime = 0, Oscillator * nosc = NULL);