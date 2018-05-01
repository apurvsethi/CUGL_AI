//
//  CUSelectorNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a selector composite behavior node.
//
//  Author: Apurv Sethi and Andrew Matsumoto
//  Version: 3/28/2018
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
 * A selector node is a composite node which is designed to run the nodes below
 * it in order, on the basis of failure for previous nodes. SelectorNode
 * "selects" one of the nodes below it as the option taken based on failure of
 * the options given as child nodes before it.
 *
 * The first node is run and if it is successful, then the SelectorNode's state
 * is set to success. Otherwise, the next node is run. If all child nodes fail,
 * then the SelectorNode has failed. It is running in the meantime.
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
	virtual std::string toString(bool verbose = false) const override;

#pragma mark -
#pragma mark Behavior Tree
	/**
	* Updates the priority value for this node and all children beneath it.
	*
	* If this node has a priority function, then the priority of this node
	* is set by calling that function. Otherwise, if this node has a child
	* that has a state other than uninitialized, this node's priority is set to
	* that child's priority. Otherwise, this node's priority is the priority of
	* the first child with a non-zero priority.
	*/
	virtual void updatePriority() override;

	/**
	 * Returns the BehaviorNode::State of the node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * behavior node (and nodes chosen to run below it in the tree).
	 *
	 * Update priority may be run as part of this function, based on whether a
	 * composite node uses preemption.
	 *
	 * @param dt	The elapsed time since the last frame.
	 *
	 * @return the BehaviorNode::State of the behavior node.
	 */
	BehaviorNode::State update(float dt) override;

#pragma mark -
#pragma mark Internal Helpers
protected:
	/**
	 * Returns the child with the smallest position which has a non-zero priority.
	 * 
	 * @return the first child with a non-zero priority.
	 */
	 const std::shared_ptr<BehaviorNode>& getSelectedChild() const;
};


}
#endif /* __CU_SELECTOR_NODE_H__ */
