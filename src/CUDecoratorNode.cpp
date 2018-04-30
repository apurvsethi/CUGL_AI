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
* Disposes all of the resources used by this node.
*
* A disposed DecoratorNode can be safely reinitialized.
*
* It is unsafe to call this on a DecoratorNode that is still currently
* inside of a running behavior tree.
*/
void DecoratorNode::dispose() {
	BehaviorNode::dispose();
	_child->setChildOffset(-1);
	_child = nullptr;
}