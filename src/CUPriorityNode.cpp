//
//  CUPriorityNode.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a priority composite behavior node.
//
//  Author: Apurv Sethi and Andrew Matsumoto
//  Version: 4/30/2018
//

#include <algorithm>
#include <sstream>
#include <cugl/ai/behaviorTree/CUPriorityNode.h>

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
std::string PriorityNode::toString(bool verbose) const {
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
* Updates the priority value for this node and all children beneath it,
* running the priority function provided or default priority function
* if available for the class.
*/
void PriorityNode::updatePriority() {
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		(*it)->updatePriority();
	}
	if (_priorityFunc) {
		setPriority(_priorityFunc());
	}
	else if (_activeChildPos != -1) {
		setPriority(_children[_activeChildPos]->getPriority());
	}
	else {
		setPriority(getMaxPriorityChild()->getPriority());
	}
}

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
	if (_state == BehaviorNode::State::RUNNING) {
		std::shared_ptr<BehaviorNode> activeChild = 
				(_activeChildPos != -1 ? _children[_activeChildPos] : nullptr);
		if (activeChild) {
			if (activeChild->getState() == BehaviorNode::State::FINISHED) {
				setState(BehaviorNode::State::FINISHED);
				return getState();
			}
			if (_preempt) {
				updatePriority();
				std::shared_ptr<BehaviorNode> maxChild = getMaxPriorityChild();
				if (maxChild != activeChild) {
					activeChild->preempt();
					_activeChildPos = maxChild->getChildOffset();
					maxChild->setState(BehaviorNode::State::RUNNING);
					activeChild = maxChild;
				}
			}
		}
		else {
			updatePriority();
			activeChild = getMaxPriorityChild();
			activeChild->preempt();
			_activeChildPos = activeChild->getChildOffset();
			activeChild->setState(BehaviorNode::State::RUNNING);
		}
		activeChild->update(dt);
	}
	return getState();
}

/**
* Returns the child with the maximum priority. Ties are broken by the
* position of the child.
*
* @return the child with the maximum priority.
*/
const std::shared_ptr<BehaviorNode>& PriorityNode::getMaxPriorityChild() const {
	return *std::max_element(_children.begin(), _children.end(),
							 BehaviorNode::compareNodeSibs);
}