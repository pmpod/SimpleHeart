#pragma once
#include "Oscillator.h"
#include <boost/heap/fibonacci_heap.hpp>
#include <boost/heap/d_ary_heap.hpp>
#include <boost/heap/pairing_heap.hpp>




struct TimestepNode
{
public:
	double timestep;
	Oscillator* osc;

};
struct compare_TimestepNode
{
	bool operator()(const TimestepNode* const& n1, const TimestepNode* const& n2) const
	{
		return n1->timestep > n2->timestep; // min heap
	}
};
//typedef  boost::heap::fibonacci_heap<TimestepNode*, boost::heap::compare<compare_TimestepNode>> stpHeap;
//typedef boost::heap::d_ary_heap<TimestepNode*, boost::heap::mutable_<true>, boost::heap::arity<2>, boost::heap::compare<compare_TimestepNode>> stpHeap;
typedef boost::heap::pairing_heap<TimestepNode*, boost::heap::compare<compare_TimestepNode>> stpHeap;



struct oscillatorToTimestepHandle
{
	Oscillator* osc;
	stpHeap::handle_type timestepHandle;
	bool inHeap;
};

TimestepNode* newTimestepNodeP(Oscillator *osc = nullptr, const double&  newTimestep = 0.2);
TimestepNode newTimestepNode(Oscillator *osc = nullptr, const double&  newTimestep = 0.2);

