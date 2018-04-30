//
//  CULeafNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a leaf behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#include <cugl/ai/behaviorTree/CULeafNode.h>
#include <sstream>

using namespace cugl;

#pragma mark -
#pragma mark Constructors
/**
 * Initializes a leaf node with the given name, priority function, and
 * action.
 *
 * @param name 		The name of the leaf node.
 * @param priority 	The priority function of the leaf node.
 * @param action 	The action of the leaf node.
 *
 * @return true if initialization was successful.
 */
bool LeafNode::init(const std::string& name, const std::function<float()>& priority,
					const std::shared_ptr<BehaviorAction>& action) {
	_name = name;
	_priorityFunc = priority;
	_action = action;
	return true;
}


/**
 * Disposes all of the resources used by this node.
 *
 * A disposed LeafNode can be safely reinitialized.
 *
 * It is unsafe to call this on a LeafNode that is still currently
 * inside of a running behavior tree.
 */
void LeafNode::dispose() {
	BehaviorNode::dispose();
	_action = nullptr;
}

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
std::string LeafNode::toString(bool verbose = false) const {
	std::stringstream ss;
	ss << (verbose ? "cugl::LeafNode(name:" : "(name:") << _name;
	ss << ", priority:" << _priority;
	ss << ", action:" << _action->getName();
	ss << ")";
	return ss.str();
}

#pragma mark -
#pragma mark Behavior Tree
/**
 * Runs the action associated with leaf node through action function.
 * If the action is not given, then nothing occurs.
 */
void LeafNode::execute() {
	if (_action) {
		_action->start();
		_state = BehaviorNode::State::RUNNING;
	}
}

/**
* Returns the BehaviorNode::State of the leaf node.
*
* Runs an update function, meant to be used on each tick, for the leaf
* node. The state for this node is derived from the state of the action
* function given. If the action function is still running, then the state
* is running. Otherwise, the state corresponds with the output of the
* action function. True implies success while false implies faliure.
*
* The priority value of the node is updated within this function, based
* on the priority function provided by the user.
*
* @param dt	The elapsed time since the last frame.
*
* @return the BehaviorNode::State of the last node.
*/
BehaviorNode::State LeafNode::update(float dt) {
	if (!_action) {
		_priority = 0;
		return _state;
	}
	switch(_state) {
		case BehaviorNode::State::UNINITIALIZED:
			if (_action->getState() == BehaviorAction::State::RUNNING) {
				_action->terminate();
			}
			break;
		case BehaviorNode::State::RUNNING:
			if (_action->getState() == BehaviorAction::State::UNINITIALIZED) {
				_action->start();
			}
			else {
				_action->update(dt);
			}
			break;
		case BehaviorNode::State::FINISHED:
			if (_action->getState() == BehaviorAction::State::RUNNING) {
				_action->terminate();
			}
	}
	_priority = _priorityFunc();
	return _state;
}