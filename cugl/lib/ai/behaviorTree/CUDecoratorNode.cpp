//
//  CUDecoratorNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a decorator behavior node.
//
//  You should never instantiate an object of this class.  Instead, you should
//  use one of the concrete subclasses of DecoratorNode. Because this is an
//  abstract class, it has no allocators.  It only has an initializer.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#include <cugl/ai/behaviorTree/CUDecoratorNode.h>

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
	_child = child;
	_name = name;
	_childOffset = -1;
	_child->setParent(this);
	_child->setChildOffset(-1);
	return true;
}

/**
 * Disposes all of the resources used by this node, and all descendants
 * in the tree.
 *
 * A disposed DecoratorNode can be safely reinitialized.
 *
 * It is unsafe to call this on a DecoratorNode that is still currently
 * inside of a running behavior tree.
 */
void DecoratorNode::dispose() {
	BehaviorNode::dispose();
	_child->dispose();
	_child = nullptr;
}

#pragma mark -
#pragma mark Behavior Tree
/**
 * Returns the BehaviorNode::State of the node.
 *
 * Runs an update function, meant to be used on each tick, for the
 * behavior node (and nodes chosen to run below it in the tree).
 *
 * Update priority may be run as part of this function, based on whether a
 * composite node uses preemption.
 *
 * @param dt	The elapsed time since the last frame.
 *
 * @return the BehaviorNode::State of the behavior node.
 */
BehaviorNode::State DecoratorNode::update(float dt) {
	if (getState() == BehaviorNode::State::UNINITIALIZED
		|| getState() == BehaviorNode::State::FINISHED) {
		return getState();
	}

	_child->setState(BehaviorNode::State::RUNNING);
	setState(_child->update(dt));
	return getState();
}

/**
 * Stops this node from running, and also stops any running nodes under
 * this node in the tree if they exist.
 */
void DecoratorNode::preempt() {
	_child->preempt();
	setState(BehaviorNode::State::UNINITIALIZED);
}
