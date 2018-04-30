//
//  CUPriorityNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a priority composite behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#include <cugl/ai/behaviorTree/CUPriorityNode.h>
#include <algorithm>
#include <sstream>

using namespace cugl;

#pragma mark -
#pragma mark Constructors
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
std::string PriorityNode::toString(bool verbose = false) const {
	std::stringstream ss;
	ss << (verbose ? "cugl::PriorityNode(name:" : "(name:") << _name;
	ss << ", priority:" << _priority;
	ss << ", children[";
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		ss << (*it)->getName() << ",";
	}
	ss << "])";
	return ss.str();
}

#pragma mark -
#pragma mark Behavior Tree
/**
* Returns the BehaviorNode::State of the priority node.
*
* Runs an update function, meant to be used on each tick, for the
* priority node (and all nodes below this node in the tree).
* The state for this node is derived from the state of the running
* or most recently run node.
*
* The priority value of the node is updated within this function, based
* on the priority values of the nodes below the given node.
*
* @param dt	The elapsed time since the last frame.
*
* @return the BehaviorNode::State of the priority node.
*/
BehaviorNode::State PriorityNode::update(float dt) {
	if (_state == BehaviorNode::State::FINISHED) {
		return _state;
	}
	std::shared_ptr<BehaviorNode> activeChild = nullptr;
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		(*it)->update(dt);
		if ((*it)->getState() != BehaviorNode::State::UNINITIALIZED) {
			activeChild = *it;
		}
	}
	if (activeChild) {
		if (_state == BehaviorNode::State::UNINITIALIZED) {
			activeChild->setState(BehaviorNode::State::UNINITIALIZED);
		}
		else if (!_preempt && activeChild->getState() == BehaviorNode::State::FINISHED) {
			_state = BehaviorNode::State::FINISHED;
		}
	}
	if (_state == BehaviorNode::State::UNINITIALIZED || !activeChild || _preempt) {
		std::shared_ptr<BehaviorNode> maxPriorityChild = *std::max_element(
			_children.begin(), _children.end(), BehaviorNode::compareNodeSibs);
		if (_state == BehaviorNode::State::RUNNING) {
			if (activeChild && maxPriorityChild != activeChild) {
				activeChild->setState(BehaviorNode::State::UNINITIALIZED);
			}
			maxPriorityChild->setState(BehaviorNode::State::RUNNING);
		}
		if (_preempt) {
			
		}
	}
}