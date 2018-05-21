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
	CUAssertLog(child, "Must pass in a child to the decorator node.");
	return BehaviorNode::init(name, nullptr, { child });
}

#pragma mark -
#pragma mark Behavior Tree
/**
 * Updates this node and any nodes under it.
 *
 * Runs an update function, meant to be used on each tick, for the
 * behavior node (and nodes below it in the tree).
 *
 * Update priority may be run as part of this function, based on whether a
 * composite node uses preemption.
 *
 * @param dt	The elapsed time since the last frame.
 *
 * @return the state of this node after updating.
 */
BehaviorNode::State DecoratorNode::update(float dt) {
	if (getState() == BehaviorNode::State::RUNNING) {
		_children[0]->setState(BehaviorNode::State::RUNNING);
		_activeChildPos = 0;
	}
	setState(_children[0]->update(dt));
	return getState();
}