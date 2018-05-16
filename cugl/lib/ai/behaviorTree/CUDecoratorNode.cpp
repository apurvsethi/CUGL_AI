//
//  CUDecoratorNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a decorator behavior node.
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

#include <cugl/ai/behaviorTree/CUDecoratorNode.h>
#include <cugl/util/CUDebug.h>

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a decorator node with the given name and child.
 *
 * @param name  The name of the decorator node.
 * @param child The child of the decorator node.
 *
 * @return true if initialization was successful.
 */
bool DecoratorNode::init(const std::string& name, const std::shared_ptr<BehaviorNode>& child) {
	return BehaviorNode::init(name, nullptr, { child });
}

#pragma mark -
#pragma mark Behavior Tree
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
BehaviorNode::State DecoratorNode::update(float dt) {
	if (getState() == BehaviorNode::State::RUNNING) {
		_children[0]->setState(BehaviorNode::State::RUNNING);
	}
	setState(_children[0]->update(dt));
	return getState();
}

/**
 * Stops this node from running, and also stops any running nodes under
 * this node in the tree if they exist.
 */
void DecoratorNode::preempt() {
	_children[0]->preempt();
	setState(BehaviorNode::State::UNINITIALIZED);
}
