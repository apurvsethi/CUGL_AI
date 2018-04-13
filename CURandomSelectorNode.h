//
//  CUSelectorNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a radnom selector composite behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_RANDOM_SELECTOR_NODE_H__
#define __CU_RANDOM_SELECTOR_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUCompositeNode.h>

namespace cugl {
	
/**
 * This class provides a selector composite node for a behavior tree.
 *
 * A random selector node is a composite node which is designed to run a
 * randomly selected nodes out of its children. The random selector finishes
 * after its selector node has also finished.
 */
class RandomSelectorNode : public CompositeNode {
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized random selector node.
	 *
	 * You must initialize this RandomSelectorNode before use.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	RandomSelectorNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~RandomSelectorNode() { dispose(); }
	
#pragma mark -
#pragma mark Static Constructors
	/**
	/**
	 * Returns a newly allocated RandomSelectorNode with the given name.
	 *
	 * @param name  The name of the priority node.
	 *
	 * @return a newly allocated RandomSelectorNode with the given name.
	 */
	static std::shared_ptr<RandomSelectorNode> alloc(const std::string& name) {
		std::shared_ptr<SelectorNode> result = std::make_shared<SelectorNode>();
		return (result->init(name) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated RandomSelectorNode with the given name and children.
	 *
	 * @param name  The name of the priority node.
	 * @param children The children of the priority node.
	 *
	 * @return a newly allocated RandomSelectorNode with the given name and children.
	 */
	static std::shared_ptr<RandomSelectorNode> allocWithChildren(const std::string& name,
														   		 const std::vector<std::shared_ptr<BehaviorNode>>& children) {
		std::shared_ptr<SelectorNode> result = std::make_shared<SelectorNode>();
		return (result->initWithChildren(name, children) ? result : nullptr);
	}

	/**
	 * Returns a newly allocated RandomSelectorNode with given name and priority
	 * function.
	 * 
	 * @param name The name of the priority node.
	 * @param priority the priority function of the priority node.
	 * 
	 * @return a newly allocated RandomSelectorNode with the given name and children. 
	 */
	static std::shared_ptr<RandomSelectorNode> allocWithPriorty(const std::string& name,,
														        const std::function<float()>& priority);

    /**
	 * Returns a newly allocated RandomSelectorNode with given name, children and 
	 * priority function.
	 * 
	 * @param name The name of the priority node.
	 * @param children The children of the priority node.
	 * @param priority the priority function of the priority node.
	 * 
	 * @return a newly allocated RandomSelectorNode with the given name and children. 
	 */
	static std::shared_ptr<RandomSelectorNode> allocWithData(const std::string& name,
															 const std::vector<std::shared_ptr<BehaviorNode>>& children,
															 const std::function<float()>& priority);

#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Returns the BehaviorNode::State of the random selector node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * random selector node (and all nodes below this node in the tree).
	 * The state for this node is derived from the state of the running
	 * or most recently run node.
	 *
	 * The priority value of the node is updated within this function or
	 * based on the priority values of the children nodes if no priority 
	 * function has been provided.
	 * 
	 * @param dt The elapsed time since the last frame.
	 * 
	 * @return the BehaviorNode::State of the selector node.
	 */
	BehaviorNode::State update(float dt) override;
};
	
	
}
#endif /* __CU_SELECTOR_NODE_H__ */
