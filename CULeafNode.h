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

namespace cugl {
	
/**
 * This class provides a leaf behavior node for a behavior tree.
 *
 * A leaf node within a behavior tree refers to the base nodes that perform actions
 * based on conditionals. Each leaf node has a user defined priority function which
 * it will call each update tick to set its priority. Additionally, each leaf node
 * also has a provided action which (TODO: Write interaction with the AI Manager.).
 */
class LeafNode : public BehaviorNode {
#pragma mark Values
protected:
	/** The state of the behavior node. 
	 * 
	 *	This function will find the current status of this leaf node.
	 *
	 *  TODO: Change to integrate with AIManager.
	 */
	std::function<BehaviorNode::State()> currentState;
	
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
	 * Initializes a leaf node with the given name and action.
	 *
	 * @param name  	The name of the leaf node.
	 * @param action	The action of the leaf node.
	 *
	 * @return true if initialization was successful.
	 */
	bool initWithAction(const std::string& name, const BehaviorAction& action);

	/**
	 * Initializes a leaf node with the given name, action, and priority
	 * function.
	 *
	 * @param name 		The name of the leaf node.
	 * @param action 	The action of the leaf node.
	 * @param priority 	The priority function of the leaf node.
	 */
	bool initWithData(const std::string& name, const BehaviorAction& action, 
					  const std::function<float()>& priority);

#pragma mark -
#pragma mark Static Constructors
	/**
	 * Allocates a leaf node with the given name.
	 *
	 * @param name  The name of the leaf node.
	 *
	 * @return a newly allocated leaf node with the given name.
	 */
	static std::shared_ptr<LeafNode> alloc(const std::string& name);
	
	/**
	 * Allocates a leaf node with the given name and action.
	 *
	 * @param name  	The name of the leaf node.
	 * @param action	The action of the leaf node.
	 *
	 * @return 	A newly allocated leaf node with the given name and action.
	 */
	static std::shared_pointer<LeafNode> allocWithAction(const std::string& name,
														 const BehaviorAction& action);

	/**
	 * Initializes a leaf node with the given name, action, and priority
	 * function.
	 *
	 * @param name 		The name of the leaf node.
	 * @param action 	The action of the leaf node.
	 * @param priority 	The priority of the leaf node.
	 * 
	 * @return 	A newly allocated leaf node with the given name, action, and 
	 * 			priority function.
	 */
	static std::shared_ptr<LeafNode> allocWithData(const std::string& name, 
								  				   const BehaviorAction& action, 
												   const std::function<float()>& priority);


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
