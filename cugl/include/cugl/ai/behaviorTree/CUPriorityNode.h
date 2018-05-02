//
//  CUPriorityNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a priority composite behavior node.
//
//  Author: Apurv Sethi and Andrew Matsumoto
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
	std::string toString(bool verbose = false) const override;

#pragma mark -
#pragma mark Internal Helpers
protected:
	/**
	 * Returns the child choosen by this priority node.
	 *
	 * A priority node will choose the child with the maximum priority, with ties broken
	 * by the child with the earliest position.
	 *
	 * @return the child choosen by this priority node.
	 */
	const std::shared_ptr<BehaviorNode>& getChosenChild() const override;
};


}
#endif /* __CU_PRIORITY_NODE_H__ */