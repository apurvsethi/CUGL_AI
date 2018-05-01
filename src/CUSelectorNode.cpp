//
//  CUSelectorNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a selector composite behavior node.
//
//  Author: Apurv Sethi and Andrew Matsumoto
//  Version: 4/30/2018
//

#include <sstream>
#include <cugl/ai/behaviorTree/CUSelectorNode.h>

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
std::string SelectorNode::toString(bool verbose) const {
	std::stringstream ss;
	ss << (verbose ? "cugl::SelectorNode(name:" : "(name:");
	ss << "priority" << _priority;
	ss << "children:[";
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		ss << (*it)->getName();
	}
	ss << "])";
	return ss.str();
}

/**
* Updates the priority value for this node and all children beneath it,
* running the piority function provided or default priority function
* if available for the class.
*/
void SelectorNode::updatePriority() {
	std::shared_ptr<BehaviorNode> selectChild = nullptr;
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		(*it)->updatePriority();
		if (!selectChild && (*it)->getPriority() != 0.0f) {
			selectChild = *it;
		}
	}
	if (_priorityFunc) {
		_priority = _priorityFunc();
		_priority = (_priority > 0 ? (_priority < 1 ? _priority : 1) : 0);
	}
	else if (_activeChildPos != -1) {
		_priority = _children[_activeChildPos]->getPriority();
	}
	else if (selectChild) {
		_priority = selectChild->getPriority();
	}
	else {
		_priority = 0;
	}
}

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
	if (_state == BehaviorNode::State::RUNNING) {
		std::shared_ptr<BehaviorNode> activeChild;
		if (_activeChildPos != -1) {
			activeChild = _children[_activeChildPos];
			if (activeChild->getState() == BehaviorNode::State::FINISHED) {
				setState(BehaviorNode::State::FINISHED);
				return getState();
			}
			if (_preempt) {
				updatePriority();
				std::shared_ptr<BehaviorNode> selectedChild = getSelectedChild();
				if (selectedChild && selectedChild != activeChild) {
					activeChild->preempt();
					_activeChildPos = selectedChild->getChildOffset();
					selectedChild->setState(BehaviorNode::State::RUNNING);
					activeChild = selectedChild;
				}
			}
			else {
				updatePriority();
				activeChild = getSelectedChild();
				_activeChildPos = activeChild->getChildOffset();
				activeChild->setState(BehaviorNode::State::RUNNING);
			}
			activeChild->update(dt);
		}
		return getState();
	}
}

#pragma mark -
#pragma mark Internal Helpers
/**
* Returns the child with the smallest position which has a non-zero priority.
*
* @return the first child with a non-zero priority.
*/
const std::shared_ptr<BehaviorNode>& SelectorNode::getSelectedChild() const {
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		if ((*it)->getPriority() > 0) {
			return *it;
		}
	}
	return nullptr;
}