//
//  CUTimerNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a decorator behavior node with a timed
//  delay.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#include <sstream>
#include <cugl/ai/behaviorTree/CUTimerNode.h>

using namespace cugl;

#pragma mark -
#pragma mark Identifiers

/**
* Disposes all of the resources used by this node.
*
* A disposed TimerNode can be safely reinitialized.
*
* It is unsafe to call this on a TimerNode that is still currently
* inside of a running behavior tree.
*/
void TimerNode::dispose() {
	DecoratorNode::dispose();
	_timeDelay = false;
	_delay = 0;
	_currentDelay = 0;
}

/**
* Initializes a timed delay node with the given name, child, delay type,
* and delay.
*
* @param name  	The name of the timer node.
* @param child 	The child of the timer node.
* @param timeDelay Whether the child node is delayed before running.
* @param delay 	The number of seconds for which the child is delayed.
*
* @return true if initialization was successful.
*/
bool TimerNode::init(const std::string& name,
					 const std::shared_ptr<BehaviorNode>& child,
					 bool timeDelay, float delay) {
	DecoratorNode::init(name, child);
	_timeDelay = timeDelay;
	_delay = delay;
	_currentDelay = 0;
	return true;
}

#pragma mark -
#pragma mark Identifier
/**
* Returns a string representation of this node for debugging purposes.
*
* If verbose is true, the string will include class information.  This
* allows us to unambiguously identify the class.
*
* @param verbose	Whether to include class information.
*
* @return a string representation of this node for debugging purposes.
*/
std::string TimerNode::toString(bool verbose) const {
	std::stringstream ss;
	ss << (verbose ? "cugl::TimerNode(name:" : "(name:") << _name;
	ss << "priority:" << _priority;
	ss << "child:" << (_child ? _child->getName() : "None");
	ss << "delay type:" << (_timeDelay ? "Before" : "After");
	ss << "delay time:" << _delay;
	ss << ")";
	return ss.str();
}
