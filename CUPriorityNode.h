//
//  CUPriorityNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a priority composite behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_PRIORITY_NODE_H__
#define __CU_PRIORITY_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUCompositeNode.h>

namespace cugl {

/**
 * This class provides a priority composite node for a behavior tree.
 *
 * A priority node is a composite node which is designed to run the nodes below
 * it with the highest priority values. A child node that is running may be
 * interrupted by another child node that has a higher priority value during the
 * update function.
 *
 * A priority node's state is directly based upon the child node currently
 * running or the child node that has finished running. Only one child node
 * will finish running as part of the PriorityNode.
 */
class PriorityNode : public CompositeNode {
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized priority node.
	 *
	 * You must initialize this PriorityNode before use.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	PriorityNode() {};

	/**
	 * Deletes this node, disposing all resources.
	 */
	~PriorityNode() { dispose(); }

#pragma mark -
#pragma mark Static Constructors

	/**
	 * Returns a newly allocated PriorityNode with the given name, children, and
	 * priority function.
	 *
	 * @param name		The name of the composite node.
	 * @param priority	The priority function of the composite node.
	 * @param children 	The children of the composite node.
	 * @param preempt	Whether child nodes can be preempted.
	 *
	 * @return a newly allocated PriorityNode with the given name, children, and
	 * priority function.
	 */
	static std::shared_ptr<PriorityNode> alloc(const std::string& name,
											   const std::function<float()> priority,
											   const std::vector<std::shared_ptr<BehaviorNode>>& children,
											   bool preempt = false) {
		std::shared_ptr<PriorityNode> result = std::make_shared<PriorityNode>();
		return (result->init(name, priority, children, preempt) ? result : nullptr);
	}

#pragma mark -
#pragma mark Identifiers
	/**
	* Returns a string representation of this node for debugging purposes.
	*
	* If verbose is true, the string will include class information.  This
	* allows us to unambiguously identify the class.
	*
	* @param verbose	Whether to include class information.
	*
	* @return a string representation of this node for debugging purposes.
	*/
	virtual std::string toString(bool verbose = false) const;

#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Returns the BehaviorNode::State of the priority node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * priority node (and all nodes below this node in the tree).
	 * The state for this node is derived from the state of the running
	 * or most recently run node.
	 *
	 * The priority value of the node is updated within this function, based
	 * on the priority values of the nodes below the given node.
	 *
	 * @param dt	The elapsed time since the last frame.
	 *
	 * @return the BehaviorNode::State of the priority node.
	 */
	BehaviorNode::State update(float dt) override;
};


}
#endif /* __CU_PRIORITY_NODE_H__ */
