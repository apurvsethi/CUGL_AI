//
//  CUFailerNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for an failer decorator behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_FAILER_NODE_H__
#define __CU_FAILER_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUDecoratorNode.h>

namespace cugl {
	
/**
 * This class provides a failer decorator node for a behavior tree.
 *
 * A failer node is a decorator node which is designed to run the node below
 * it and set its state to failure regardless of the state of its child node.
 *
 * An failer node's priority is directly based upon the child node's priority.
 */
class FailerNode : public DecoratorNode {
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized failer node.
	 *
	 * You must initialize this FailerNode before use.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	FailerNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~FailerNode() { dispose(); }
	
#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated FailerNode with the given name.
	 *
	 * @param name  The name of the failer node.
	 *
	 * @return a newly allocated FailerNode with the given name.
	 */
	static std::shared_ptr<FailerNode> alloc(const std::string& name) {
		std::shared_ptr<FailerNode> result = std::make_shared<FailerNode>();
		return (result->init(name) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated FailerNode with the given name and child.
	 *
	 * @param name  The name of the failer node.
	 * @param child The child of the failer node.
	 *
	 * @return a newly allocated FailerNode with the given name and child.
	 */
	static std::shared_ptr<FailerNode> allocWithChild(const std::string& name,
													  const std::shared_ptr<BehaviorNode>& child) {
		std::shared_ptr<FailerNode> result = std::make_shared<FailerNode>();
		return (result->initWithChild(name, child) ? result : nullptr);
	}
	
#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Returns the BehaviorNode::State of the failer node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * failer node (and all nodes below this node in the tree).
	 * The state for this node is derived from the state of its child node.
	 *
	 * The priority value of the node is updated within this function, based
	 * on the priority values of the child node.
	 *
	 * @return the BehaviorNode::State of the child node.
	 */
	BehaviorNode::State update() override;
};
	
	
}
#endif /* __CU_FAILER_NODE_H__ */
