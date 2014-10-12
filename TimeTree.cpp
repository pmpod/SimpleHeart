#include "TimeTree.h"
//#include <assert.h>


TimeTree *newTreeNode(const double& newUpdateTime, TimeTree * parent, Oscillator *osc)
{
	TimeTree *node = new TimeTree;

	node->osc = osc;
	node->laterNode = nullptr;
	node->earlierNode = nullptr;
	node->parentNode = parent;
	node->time = newUpdateTime;
	return node;
}
//---------------------------------------------------------------------------------
void clear(TimeTree *&node)
{
	if (node != nullptr) {
		clear(node->earlierNode);
		clear(node->laterNode);
		delete node;
	}
}
//---------------------------------------------------------------------------------
TimeTree *newTreeTrunk(TimeTree *&node, vector<Oscillator*>& mesh, vector<oscillatorToUpdateNode>& dictionary, double initValue)
{
	int numberOfAdded = 0;
	dictionary.clear();
	int treeDepth = ceil(log(static_cast<double>(mesh.size())) / log(2.0));
	return newTreeTrunk(node, mesh, numberOfAdded, dictionary, treeDepth, initValue);
}
TimeTree *newTreeTrunk(TimeTree *&node, vector<Oscillator*>& mesh, int& numberOfOscillatorsAdded, vector<oscillatorToUpdateNode>& dictionary, int treeDepth, double initValue)
{
	//if (treeDepth == 0)
	//{
	//	assert("modelDepth == 0");
	//	return nullptr;
	//}
	//if (treeDepth == 1)
	//{
	//	assert("modelDepth == 1");
	//	return node;
	//}
	if (node == nullptr)
	{
		//assert("node == nullptr");
		return nullptr;
	}

	if (node != nullptr)
	{
		if ((treeDepth) > 0)
		{
			if (node->earlierNode == nullptr)
			{
				node->earlierNode = newTreeNode(initValue, node);
				//newTreeTrunk(node->earlierNode, treeDepth - 1, initValue);
				newTreeTrunk(node->earlierNode, mesh, numberOfOscillatorsAdded, dictionary, treeDepth - 1, initValue);
			}
			else
			{
				//assert("Tree was occupied!");
			}
			if (node->laterNode == nullptr)
			{
				node->laterNode = newTreeNode(initValue, node);
				//newTreeTrunk(node->laterNode, treeDepth - 1, initValue);
				newTreeTrunk(node->laterNode, mesh, numberOfOscillatorsAdded, dictionary, treeDepth - 1, initValue);
			}
			else
			{
				//assert("Tree was occupied!");
			}
		}
		if (treeDepth == 0)
		{
			//tutaj mozna dodac oscylator
			if (numberOfOscillatorsAdded < mesh.size())
			{
				node->osc = mesh[numberOfOscillatorsAdded];
				++numberOfOscillatorsAdded;

				//dictionary update
				oscillatorToUpdateNode nd;
				nd.osc = node->osc;
				nd.p_updateTimeTreeNode = node;
				dictionary.push_back(nd);
			}
		}
	}
	return node;
}
TimeTree *newTreeTrunk(TimeTree*& node, const int& modelDepth, double initValue)
{
	if (modelDepth == 0)
	{
		//assert("modelDepth == 0");
		return nullptr;
	}
	if (modelDepth == 1)
	{
		//assert("modelDepth == 1");
		return node;
	}
	if (node == nullptr)
	{
		//assert("node == nullptr");
		return nullptr;
	}


	if (node != nullptr)
	{
		if ((modelDepth - 1) > 0)
		{
			if (node->earlierNode == nullptr)
			{
				node->earlierNode = newTreeNode(initValue, node);
				newTreeTrunk(node->earlierNode, modelDepth - 1, initValue);
			}
			else
			{
				//assert("Tree was occupied!");
			}
			if (node->laterNode == nullptr)
			{
				node->laterNode = newTreeNode(initValue, node);
				newTreeTrunk(node->laterNode, modelDepth - 1, initValue);
			}
			else
			{
				//assert("Tree was occupied!");
			}
		}
		else if (modelDepth == 0)
		{
			//tutaj mozna dodac oscylator
		}
	}
	return node;
}
//---------------------------------------------------------------------------------
TimeTree *resetTime(TimeTree *node, double val)
{
	if (node == nullptr)
	{
		//assert("node == nullptr");
		return nullptr;
	}


	if (node != nullptr)
	{
		node->time = val;

		if (node->earlierNode != nullptr)
		{
			resetTime(node->earlierNode, val);
		}
		if (node->laterNode != nullptr)
		{
			resetTime(node->laterNode, val);
		}
		if ((node->earlierNode == nullptr) && (node->laterNode == nullptr) && (node->osc == nullptr))
		{
			node->time = DBL_MAX;
		}
		bubbleNewTime(node, node->time);
	}
	return node;
}
//---------------------------------------------------------------------------------
void swapChildrenIfNecessary(TimeTree *&node)
 {
	// if (node != nullptr && node->earlierNode != nullptr && node->earlierNode != nullptr) 
	//if ( node->earlierNode != nullptr)
}
void bubbleNewTime(TimeTree *&node, const double& newTime)//worksOnlyOnBottomNode!
{

	if (node->earlierNode != nullptr)
	{

		if (node->earlierNode->time > node->laterNode->time)
			std::swap(node->earlierNode, node->laterNode);
		node->time = node->earlierNode->time;
	}
	else
	{
		node->time = newTime;
	}
	if (node->parentNode != nullptr)
	{
		bubbleNewTime(node->parentNode, node->time);
	}
}
TimeTree *goToEarliest(TimeTree *node)
 {
		 if (node->earlierNode != nullptr)
		 {
			 return goToEarliest(node->earlierNode);
		 }
		 else
			 return node;
}
//---------------------------------------------------------------------------------
TimeTree* addOscillator(TimeTree *&node, Oscillator* nosc, double initValue)
 {
	TimeTree* p = nullptr;
	 if (node != nullptr)
	 {
		// if (node->earlierNode != nullptr && node->laterNode != nullptr)
		 if (node->earlierNode != nullptr)
		 {
			 p = addOscillator(node->laterNode, nosc, initValue);
			 if (p == nullptr)
			 {
				 p = addOscillator(node->earlierNode, nosc, initValue);
			 }
		 }
		 else if (node->osc == nullptr)
		 {
			 node->osc = nosc;
			 node->time = initValue;
			 //swapChildrenIfNecessary(node->parentNode);
			 //bubbleNewTime(node, initValue);
			 //node->osc->p_timestepTreeNode = node;
			 return node;
		 }
		 else
		 {
			 return nullptr;
		 }
	 }
	 else
	 {
		 //assert("node = nullptr in addOscillator");
	 }
	 return p;
	 //TimeTree* p = nullptr;
	 //if (node != nullptr)
	 //{
		// // if (node->earlierNode != nullptr && node->laterNode != nullptr)
		// if (node->earlierNode != nullptr)
		// {
		//	 p = addOscillator(node->laterNode, nosc, initValue);
		//	 if (p == nullptr)
		//	 {
		//		 p = addOscillator(node->earlierNode, nosc, initValue);
		//	 }
		//	 if (p == nullptr)
		//	 {
		//		 node->time = initValue;
		//		 if (node->parentNode != nullptr)
		//			 swapChildrenIfNecessary(node->parentNode);
		//	 }
		// }
		// else if (node->osc == nullptr)
		// {
		//	 node->osc = nosc;
		//	 node->time = initValue;
		//	 swapChildrenIfNecessary(node->parentNode);
		//	 //bubbleNewTime(node, initValue);
		//	 //node->osc->p_timestepTreeNode = node;
		//	 return node;
		// }
		// else
		// {
		//	 return nullptr;
		// }
	 //}
	 //else
	 //{
		// assert("node = nullptr in addOscillator");
	 //}
	 //return p;
 }
TimeTree *goToGreaterThan(TimeTree *node, const double& val)
{
	if (node != nullptr)
	{
		if (node->earlierNode != nullptr && node->laterNode != nullptr)
		{
			if (node->earlierNode->time > val)
				return goToGreaterThan(node->earlierNode, val);
			else
				return goToGreaterThan(node->laterNode, val);
		}
		else
		{
			if (node->time > val)
				return node;
			else
				return nullptr;
		}
	}
	else
		return nullptr;

}
