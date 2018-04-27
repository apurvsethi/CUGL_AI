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

#include <cugl/ai/behaviorTree/CUBehaviorTree.h>

using namespace cugl;

/**
 * Disposes all of the resources used by this node.
 *
 * A disposed DecoratorNode can be safely reinitialized.
 *
 * It is unsafe to call this on a DecoratorNode that is still currently
 * inside of a running behavior tree.
 */
void DecoratorNode::dispose() {
    BehaviorNode::dispose();
    _child = nullptr;
    //TODO: Remove from parent.
}

/**
 * Initializes a decorator node using the given template def.
 *
 * @param behaviorNodeDef	The def specifying arguments for this node.
 *
 * @return true if initialization was successful.
 */
bool DecoratorNode::init(const std::shared_ptr<BehaviorNodeDef>& behaviorNodeDef) {
    if (behaviorNodeDef->childCount != 1) {
        return false;
    }
    BehaviorNode::init(behaviorNodeDef);
    //TODO: Initialize child.
}