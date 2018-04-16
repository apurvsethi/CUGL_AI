//
//  CULeafNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a leaf behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_LEAF_NODE_H__
#define __CU_LEAF_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUBehaviorNode.h>
#include <cugl/ai/behaviorTree/CUBehaviorAction.h>

namespace cugl {
	
/**
 * This class provides a leaf behavior node for a behavior tree.
 *
 * A leaf node within a behavior tree refers to the base nodes that perform
 * actions based on conditionals. Each leaf node has a user defined priority
 * function which it will call each update tick to set its priority.
 * Additionally, each leaf node also has a provided action which begins running
 * when the leaf node is chosen for execution.
 */
class LeafNode : public BehaviorNode {
#pragma mark Values
protected:
	/**
	 * The action used when this node is run.
	 *
	 * This should only be used when this node is of type LeafNode.
	 */
	std::shared_ptr<BehaviorAction> _action;
	
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized leaf node.
	 *
	 * You must initialize this LeafNode before use.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	LeafNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~LeafNode() { dispose(); }
	
	/**
	 * Disposes all of the resources used by this node.
	 *
	 * A disposed LeafNode can be safely reinitialized.
	 *
	 * It is unsafe to call this on a LeafNode that is still currently
	 * inside of a running behavior tree.
	 */
	void dispose() override;
	
	/**
	 * Initializes a leaf node using the given template def.
	 *
	 * @param behaviorNodeDef	The def specifying arguments for this node.
	 *
	 * @return true if initialization was successful.
	 */
	bool init(const std::shared_ptr<BehaviorNodeDef>& behaviorNodeDef) override;

#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated LeafNode using the given template def.
	 *
	 * @param behaviorNodeDef	The def specifying arguments for this node.
	 *
	 * @return a newly allocated LeafNode using the given template def.
	 */
	static std::shared_ptr<LeafNode> alloc(const std::shared_ptr<BehaviorNodeDef>& behaviorNodeDef) {
		std::shared_ptr<LeafNode> result = std::make_shared<LeafNode>();
		return (result->init(behaviorNodeDef) ? result : nullptr);
	}

#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Runs the action associated with leaf node through action function.
	 * If the action is not given, then nothing occurs.
	 */
	void execute();
	
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
	 * @return the BehaviorNode::State of the laf node.
	 */
	BehaviorNode::State update(float dt) override;
};
	
	
}
#endif /* __CU_LEAF_NODE_H__ */
