//
//  CUSelectorNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a selector composite behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#include <cugl/ai/behaviorTree/CUSelectorNode.h>

using namespace cugl;

/**
 * Returns the BehaviorNode::State of the selector node.
 *
 * Runs an update function, meant to be used on each tick, for the
 * selector node(and all nodes below this node in the tree).
 * The state for this node is derived from the state of the running
 * or most recently run node.
 *
 * The priority value of the node is updated within this function, based
 * on the priority values of the nodes below the given node.
 *
 * @param dt	The elapsed time since the last frame.
 *
 * @return the BehaviorNode::State of the selector node.
 */
BehaviorNode::State SelectorNode::update(float dt) {
	if (_state == BehaviorNode::State::FINISHED) {
		return _state;
	}
	std::shared_ptr<BehaviorNode> activeChild = nullptr, selectChild = nullptr;
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		(*it)->update(dt);
		if (!selectChild && (*it)->getPriority() > 0) {
			selectChild = *it;
		}
		if ((*it)->getState() != BehaviorNode::State::UNINITIALIZED) {
			activeChild = *it;
		}
	}
	if (_state == BehaviorNode::State::UNINITIALIZED) {
		if (activeChild) {
			activeChild->setState(BehaviorNode::State::UNINITIALIZED);
		}
		_priority = _priorityFunc ? _priorityFunc() : 
			(selectChild ? selectChild->getPriority() : 0);
	}
	else if (_state == BehaviorNode::State::RUNNING) {
		if (!activeChild) {
			selectChild->start();
			activeChild = selectChild;
		}
		else if (selectChild && activeChild != selectChild && _preempt) {
			activeChild->setState(BehaviorNode::State::UNINITIALIZED);
			selectChild->start();
			activeChild = selectChild;
		}
		else if (activeChild->getState() == BehaviorNode::State::FINISHED) {
			_state = BehaviorNode::State::FINISHED;
		}
		_priority = _priorityFunc ? _priorityFunc() :
			(activeChild ? activeChild->getPriority() : 0);
	}
	return _state;
}