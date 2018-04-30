//
//  CUInverterNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for an inverter decorator behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#include <sstream>
#include <cugl/ai/behaviorTree/CUInverterNode.h>

using namespace cugl;

#pragma mark -
#pragma mark Identifiers
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
std::string InverterNode::toString(bool verbose = false) const {
	std::stringstream ss;
	ss << (verbose ? "cugl::InverterNode(name:" : "(name:") << _name;
	ss << ",priority:" << _priority;
	ss << ",child:" << (_child ? _child->getName() : "None");
	ss << ")";
	return ss.str();
}

#pragma mark -
#pragma mark Behavior Tree

/**
* Returns the BehaviorNode::State of the inverter node.
*
* Runs an update function, meant to be used on each tick, for the
* inverter node (and all nodes below this node in the tree).
* The state for this node is derived from the state of its child node.
*
* The priority value of the node is updated within this function, based
* on the priority values of the child node.
*
* @param dt	The elapsed time since the last frame.
*
* @return the BehaviorNode::State of the child node.
*/
BehaviorNode::State InverterNode::update(float dt) {
	switch (_state) {
	case BehaviorNode::State::RUNNING:
		_state = _child->update(dt);
		_priority = 1 - _child->getPriority();
		break;
	case BehaviorNode::State::UNINITIALIZED:
		_child->setState(BehaviorNode::State::UNINITIALIZED);
		_child->update(dt);
		_priority = 1 - _child->getPriority();
		break;
	}
	return _state;
}