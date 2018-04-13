//
//  CUSelectorNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a selector composite behavior node.
//
//  Author: Apurv Sethi
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
 * it in order, on the basis of failure for previous nodes. SelectorNode "selects"
 * one of the nodes below it as the option taken based on failure of the options
 * given as child nodes before it.
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
	SelectorNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~SelectorNode() { dispose(); }
	
#pragma mark -
#pragma mark Static Constructors
	/**
	/**
	 * Returns a newly allocated SelectorNode with the given name.
	 *
	 * @param name  The name of the priority node.
	 *
	 * @return a newly allocated SelectorNode with the given name.
	 */
	static std::shared_ptr<SelectorNode> alloc(const std::string& name) {
		std::shared_ptr<SelectorNode> result = std::make_shared<SelectorNode>();
		return (result->init(name) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated SelectorNode with the given name and children.
	 *
	 * @param name  The name of the priority node.
	 * @param children The children of the priority node.
	 *
	 * @return a newly allocated SelectorNode with the given name and children.
	 */
	static std::shared_ptr<SelectorNode> allocWithChildren(const std::string& name,
														   const std::vector<std::shared_ptr<BehaviorNode>>& children) {
		std::shared_ptr<SelectorNode> result = std::make_shared<SelectorNode>();
		return (result->initWithChildren(name, children) ? result : nullptr);
	}

	/**
	 * Returns a newly allocated SelectorNode with given name and priority
	 * function.
	 * 
	 * @param name The name of the priority node.
	 * @param priority the priority function of the priority node.
	 * 
	 * @return a newly allocated SelectorNode with the given name and children. 
	 */
	static std::shared_ptr<SelectorNode> allocWithPriorty(const std::string& name,,
														  const std::function<float()>& priority);

    /**
	 * Returns a newly allocated SelectorNode with given name, children and 
	 * priority function.
	 * 
	 * @param name The name of the priority node.
	 * @param children The children of the priority node.
	 * @param priority the priority function of the priority node.
	 * 
	 * @return a newly allocated SelectorNode with the given name and children. 
	 */
	static std::shared_ptr<SelectorNode> allocWithData(const std::string& name,
													   const  std::vector<std::shared_ptr<BehaviorNode>>& children,
													   sconst std::function<float()>& priority);

#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Returns the BehaviorNode::State of the selector node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * selector node (and all nodes below this node in the tree).
	 * The state for this node is derived from the state of the running
	 * or most recently run node.
	 *
	 * The priority value of the node is updated within this function, based
	 * on the priority values of the nodes below the given node.
	 *
	 * @param dt The elapsed time since the last frame.
	 * 
	 * @return the BehaviorNode::State of the selector node.
	 */
	BehaviorNode::State update(float dt) override;
};
	
	
}
#endif /* __CU_SELECTOR_NODE_H__ */
