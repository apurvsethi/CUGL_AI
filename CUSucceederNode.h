//
//  CUSucceederNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for an succeeder decorator behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_SUCCEEDER_NODE_H__
#define __CU_SUCCEEDER_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUDecoratorNode.h>

namespace cugl {
	
/**
 * This class provides a succeeder decorator node for a behavior tree.
 *
 * A succeeder node is a decorator node which is designed to run the node below
 * it and set its state to success regardless of the state of its child node.
 *
 * An succeeder node's priority is directly based upon the child node's priority.
 */
class SucceederNode : public DecoratorNode {
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized succeeder node.
	 *
	 * You must initialize this SucceederNode before use.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	SucceederNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~SucceederNode() { dispose(); }
	
#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated SucceederNode with the given name.
	 *
	 * @param name  The name of the succeeder node.
	 *
	 * @return a newly allocated SucceederNode with the given name.
	 */
	static std::shared_ptr<SucceederNode> alloc(const std::string& name) {
		std::shared_ptr<SucceederNode> result = std::make_shared<SucceederNode>();
		return (result->init(name) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated SucceederNode with the given name and child.
	 *
	 * @param name  The name of the succeeder node.
	 * @param child The child of the succeeder node.
	 *
	 * @return a newly allocated SucceederNode with the given name and child.
	 */
	static std::shared_ptr<SucceederNode> allocWithChild(const std::string& name,
														 const std::shared_ptr<BehaviorNode>& child) {
		std::shared_ptr<SucceederNode> result = std::make_shared<SucceederNode>();
		return (result->initWithChild(name, child) ? result : nullptr);
	}
	
#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Returns the BehaviorNode::State of the succeeder node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * succeeder node (and all nodes below this node in the tree).
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
#endif /* __CU_SUCCEEDER_NODE_H__ */
