#include "NumericStrategy\TimestepNode.h"

\
TimestepNode *newTimestepNodeP(Oscillator *osc, const double&  newTimestep)
{
	TimestepNode *node = new TimestepNode;
	//node->updated = false;
	node->osc = osc;
	node->timestep = newTimestep;
	return node;
}
TimestepNode newTimestepNode(Oscillator *osc, const double&  newTimestep)
{
	TimestepNode node;

	node.osc = osc;
	node.timestep = newTimestep;
	return node;
}