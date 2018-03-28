//
//  CUSequenceNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a sequence composite behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_SEQUENCE_NODE_H__
#define __CU_SEQUENCE_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUCompositeNode.h>

namespace cugl {
	
/**
 * This class provides a sequence composite node for a behavior tree.
 *
 * A sequence node is a composite node which is designed to run the nodes below
 * it in order, on the basis of success for previous nodes.
 *
 * The first child is run, and if it is successful, then the next child is run.
 * If any child fails, then the SequenceNode fails, while the SequenceNode succeeds
 * if all children running in sequence succeed. The node is running in the meantime.
 */
class SequenceNode : public CompositeNode {
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized sequence node.
	 *
	 * You must initialize this SequenceNode before use.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	SequenceNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~SequenceNode() { dispose(); }
	
#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated SequenceNode with the given name.
	 *
	 * @param name  The name of the sequence node.
	 *
	 * @return a newly allocated SequenceNode with the given name.
	 */
	static std::shared_ptr<SequenceNode> alloc(const std::string& name) {
		std::shared_ptr<SequenceNode> result = std::make_shared<SequenceNode>();
		return (result->init(name) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated SequenceNode with the given name and children.
	 *
	 * @param name  The name of the sequence node.
	 * @param children The children of the sequence node.
	 *
	 * @return a newly allocated SequenceNode with the given name and children.
	 */
	static std::shared_ptr<SequenceNode> allocWithChildren(const std::string& name,
														   const std::vector<std::shared_ptr<BehaviorNode>>& children) {
		std::shared_ptr<SequenceNode> result = std::make_shared<SequenceNode>();
		return (result->initWithChildren(name, children) ? result : nullptr);
	}
	
#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Returns the BehaviorNode::State of the sequence node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * sequence node (and all nodes below this node in the tree).
	 * The state for this node is derived from the state of the running
	 * or most recently run node.
	 *
	 * The sequence value of the node is updated within this function, based
	 * on the sequence values of the nodes below the given node.
	 *
	 * @return the BehaviorNode::State of the sequence node.
	 */
	BehaviorNode::State update() override;
};
	
	
}
#endif /* __CU_SEQUENCE_NODE_H__ */
