//
//  CUSelectorNode.h
//  Cornell University Game Library (CUGL)
//
//  This class uses our standard shared-pointer architecture.
//
//  1. The constructor does not perform any initialization; it just sets all
//     attributes to their defaults.
//
//  2. All initialization takes place via init methods, which can fail if an
//     object is initialized more than once.
//
//  3. All allocation takes place via static constructors which return a shared
//     pointer.
//
//  Author: Apurv Sethi and Andrew Matsumoto
//  Version: 5/21/2018
//

#ifndef __CU_SELECTOR_NODE_H__
#define __CU_SELECTOR_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUCompositeNode.h>

namespace cugl {

/**
 * This class provides a selector composite node for a behavior tree.
 *
 * A selector node is a composite node which is designed to select and
 * run the first child with a non-zero priority and run it. If the selector
 * node is allowed to preempt, a child that is running may be overridden by an
 * earlier child with a non-zero priority during the update function.
 *
 * If the selector node is not assigned a priority function, its priority
 * will be assigned as the running child if this node is currently running,
 * or as the the first child with a non-zero priority. If all children have
 * a priority of 0, then this node's priority will also be 0.
 *
 * A selector node's state is directly based upon the child node currently
 * running or the child node that has finished running. Only one child node
 * will finish running as part of the SelectorNode.
 */
class SelectorNode : public CompositeNode {
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized selector node.
	 *
	 * You must initialize this SelectorNode before use.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	SelectorNode() {};

	/**
	 * Deletes this node, disposing all resources.
	 */
	~SelectorNode() { dispose(); }

#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated SelectorNode with the given name, children, and
	 * priority function.
	 *
	 * @param name		The name of the composite node.
	 * @param priority	The priority function of the composite node.
	 * @param children 	The children of the composite node.
	 * @param preempt	Whether child nodes can be preempted.
	 *
	 * @return a newly allocated SelectorNode with the given name, children, and
	 * priority function.
	 */
	static std::shared_ptr<SelectorNode> alloc(const std::string& name,
											   const std::function<float()> priority,
											   const std::vector<std::shared_ptr<BehaviorNode>>& children,
											   bool preempt) {
		std::shared_ptr<SelectorNode> result = std::make_shared<SelectorNode>();
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
	 * Returns the child choosen by this selector node.
	 *
	 * A selector node will choose the first child with a nonzero priority. If all
	 * children have a priority of zero, the selector node will choose the first
	 * child.
	 *
	 * @return the child choosen by this selector node.
	 */
	const std::shared_ptr<BehaviorNode>& getChosenChild() const override;
};


}
#endif /* __CU_SELECTOR_NODE_H__ */
