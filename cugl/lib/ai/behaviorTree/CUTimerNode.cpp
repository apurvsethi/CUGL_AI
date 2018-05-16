//
//  CUTimerNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a decorator behavior node with a timed
//  delay.
//
//  This class uses our standard shared-pointer architecture.
//
//  1. The constructor does not perform any initialization; it just sets all
//     attributes to their defaults.
//
//  2. All initialization takes place via init methods, which can fail if an
//     object is initialized more than once.
//
//  3. All allocation takes place via static constructors which return a shared
//     pointer.
//
//  Author: Apurv Sethi and Andrew Matsumoto
//  Version: 5/16/2018
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
	_delay = 0.0f;
	_delaying = false;
	_currentDelay = 0.0f;
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
	_delaying = false;
	_currentDelay = 0.0f;
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
	ss << "child:" << (_children[0] ? _children[0]->getName() : "None");
	ss << "delay type:" << (_timeDelay ? "Before" : "After");
	ss << "delay time:" << _delay;
	ss << ")";
	return ss.str();
}

#pragma mark -
#pragma mark Behavior Tree
/**
 * Sets the state of this node.
 *
 * This state is used to identify the state of the node. If the node
 * has no parent, then this is the state of the behavior tree.
 *
 * @param state The state of this node.
 */
void TimerNode::setState(BehaviorNode::State state) {
	if (state == BehaviorNode::State::RUNNING && _timeDelay) {
		_delaying = true;
	}
	_state = state;
}

/**
 * Updates the priority value for this node and all children beneath it,
 * running the piority function provided or default priority function
 * if available for the class.
 */
void TimerNode::updatePriority() {
	if (_delaying && !_timeDelay) {
		setPriority(0.0f);
	}
	else {
		_children[0]->updatePriority();
		setPriority(_children[0]->getPriority());
	}
}

/**
 * Updates this node and its child.
 *
 * Runs an update function, meant to be used on each tick, for the
 * behavior node (and nodes chosen to run below it in the tree).
 *
 * @param dt	The elapsed time since the last frame.
 *
 * @return the BehaviorNode::State of the behavior node.
 */
BehaviorNode::State TimerNode::update(float dt) {
	if (_delaying) {
		_currentDelay += dt;
		if (_currentDelay >= _delay) {
			_delaying = false;
			_currentDelay = 0.0f;
		}
	}

	if (_delaying && _timeDelay) {
		return getState();
	}
	return DecoratorNode::update(dt);
}

/**
 * Stops this node from running, and also stops any running nodes under
 * this node in the tree if they exist.
 */
void TimerNode::preempt() {
	if (!_timeDelay) {
		_delaying = true;
	}
	DecoratorNode::preempt();
}
