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

#include <functional>
#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUBehaviorNode.h>

namespace cugl {
	
/**
 * This class provides a leaf behavior node for a behavior tree.
 *
 * A leaf node within a behavior tree refers to the base nodes that perform actions
 * based on conditionals. set of nodes that have
 * multiple children under them and run the children in some order. There are rules
 * specific to each type of composite node defining how many children are run,
 * the sequence in which they are run, and the definition of sucess or failure.
 *
 * The three concrete subclasses for a CompositeNode are: PriorityNode,
 * SequenceNode, and SelectorNode. While similar in structure, each class has key
 * differences defining how they run in relation to their child nodes.
 */
class LeafNode : public BehaviorNode {
#pragma mark Values
protected:
	/** The priority function used to obtain the priority of this leaf node. */
	std::function<float()> priority;
	
	/** The action represented by this leaf node. */
	std::function<bool()> action;
	
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
	 * Initializes a leaf node with the given name.
	 *
	 * @param name  The name of the leaf node.
	 *
	 * @return true if initialization was successful.
	 */
	bool init(const std::string& name) override;
	
	/**
	 * Initializes a leaf node with the given name, priority function,
	 * and action function.
	 *
	 * @param name  	The name of the leaf node.
	 * @param priority	The priority function of the leaf node.
	 * @param action	The action function of the leaf node.
	 *
	 * @return true if initialization was successful.
	 */
	bool initWithFunctions(const std::string& name, std::function<float()> priority,
						   std::function<bool()> action);
	
#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated LeafNode with the given name.
	 *
	 * @param name  The name of the leaf node.
	 *
	 * @return a newly allocated LeafNode with the given name.
	 */
	static std::shared_ptr<LeafNode> alloc(const std::string& name) {
		std::shared_ptr<LeafNode> result = std::make_shared<LeafNode>();
		return (result->init(name) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated LeafNode with the given name, priority function,
	 * and action function.
	 *
	 * @param name  	The name of the leaf node.
	 * @param priority	The priority function of the leaf node.
	 * @param action	The action function of the leaf node.
	 *
	 * @return a newly allocated LeafNode with the given name.
	 */
	static std::shared_ptr<LeafNode> allocWithFunctions(const std::string& name,
														std::function<float()> priority,
														std::function<bool()> action) {
		std::shared_ptr<LeafNode> result = std::make_shared<LeafNode>();
		return (result->initWithFunctions(name, priority, action) ? result : nullptr);
	}
	
#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Begins the action associated with leaf node through action function.
	 * If the action function is not given, then nothing occurs.
	 */
	void startAction();
	
	/**
	 * Returns the BehaviorNode::State of the leaf node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * leaf node. The state for this node is derived from the state of the
	 * action function given. If the action function is still running, then
	 * the state is running. Otherwise, the state corresponds with the output
	 * of the action function. True implies success while false implies faliure.
	 *
	 * The priority value of the node is updated within this function, based
	 * on the priority function provided by the user.
	 *
	 * @return the BehaviorNode::State of the laf node.
	 */
	BehaviorNode::State update() override;
};
	
	
}
#endif /* __CU_LEAF_NODE_H__ */
