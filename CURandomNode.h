//
//  CURandomNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a random composite behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_RANDOM_NODE_H__
#define __CU_RANDOM_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUCompositeNode.h>

namespace cugl {
	
/**
 * This class provides a random composite node for a behavior tree.
 *
 * A random node is a composite node which is designed to run a randomly
 * selected nodes out of its children, based on either uniform probability or
 * weighted probability. The random node finishes after its selected node has
 * also finished.
 */
class RandomNode : public CompositeNode {
#pragma mark Values
protected:
	/**
	 * Whether or not the random node should choose the child for execution
	 * based on a uniformly at random choice amongst its children, or should
	 * choose the child randomly with weightage being provided for children
	 * nodes through their priority values. If true, then the node chooses
	 * uniformly at random, otherwise the node uses a weighted probability.
	 */
	bool _uniformRandom;

#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized random node.
	 *
	 * You must initialize this RandomNode before use.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	RandomNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~RandomNode() { dispose(); }
	
#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated RandomNode using the given template def.
	 *
	 * @param behaviorNodeDef	The def specifying arguments for this node.
	 *
	 * @return a newly allocated RandomNode using the given template def.
	 */
	static std::shared_ptr<RandomNode> alloc(const std::shared_ptr<BehaviorNodeDef>& behaviorNodeDef) {
		std::shared_ptr<RandomNode> result = std::make_shared<RandomNode>();
		return (result->init(behaviorNodeDef) ? result : nullptr);
	}

#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Returns the BehaviorNode::State of the random node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * random node (and all nodes below this node in the tree). The state for
	 * this node is derived from the state of the running or most recently run
	 * node.
	 *
	 * The priority value of the node is updated within this function or
	 * based on the priority values of the children nodes if no priority 
	 * function has been provided.
	 * 
	 * @param dt	The elapsed time since the last frame.
	 * 
	 * @return the BehaviorNode::State of the selector node.
	 */
	BehaviorNode::State update(float dt) override;
};
	
	
}
#endif /* __CU_RANDOM_NODE_H__ */
