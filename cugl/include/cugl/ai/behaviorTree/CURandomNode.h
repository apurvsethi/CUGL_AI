//
//  CURandomNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a random composite behavior node.
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
 * selected nodes out of its children, based on either a uniform probability or
 * a weighted probability. A random node using a weighted probability will base
 * the weights of the probability of selecting each child on the priority of
 * that child.
 *
 * If a random node is not given a priority function, it will set its priority
 * as the average of the priorities of its children.
 *
 * A random node's state is directly based upon the child node currently
 * running or the child node that has finished running. Only one child node
 * will finish running as part of the RandomNode.
 */
class RandomNode : public CompositeNode {
#pragma mark Values
protected:
	/**
	 * Whether this node should choose the child to execute based uniformly
	 * at random.
	 *
	 * If true, then this node chooses its child uniformly at random. Otherwise,
	 * this node uses a weighted probability among its children based on each
	 * child's priority value.
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
	RandomNode() : _uniformRandom(true) {};

	/**
	 * Deletes this node, disposing all resources.
	 */
	~RandomNode() { dispose(); }

	/**
	 * Disposes all of the resources used by this node.
	 *
	 * A disposed RandomNode can be safely reinitialized. Any children owned
	 * by this node will be released. They will be deleted if no other object
	 * owns them.
	 *
	 * It is unsafe to call this on a random node that is still currently
	 * inside of a running behavior tree.
	 */
	void dispose() override;

	/**
	 * Initializes a random node with the given name, children, and priority
	 * function. Utilizes uniformly at random to choose child nodes.
	 *
	 * @param name			The name of the random node.
	 * @param priority		The priority function of the random node.
	 * @param children 		The children of the random node.
	 * @param preempt		Whether child nodes can be preempted.
	 *
	 * @return true if initialization was successful.
	 */
	bool init(const std::string& name,
			  const std::function<float()> priority,
			  const std::vector<std::shared_ptr<BehaviorNode>>& children,
			  bool preempt) override {
		return init(name, priority, children, preempt, true);
	}

	/**
	 * Initializes a random node with the given name, children, priority
	 * function, and random type.
	 *
	 * @param name			The name of the random node.
	 * @param priority		The priority function of the random node.
	 * @param children 		The children of the random node.
	 * @param preempt		Whether child nodes can be preempted.
	 * @param uniformRandom	Whether children are chosen uniformly at random.
	 *
	 * @return true if initialization was successful.
	 */
	bool init(const std::string& name,
			  const std::function<float()> priority,
			  const std::vector<std::shared_ptr<BehaviorNode>>& children,
			  bool preempt, bool uniformRandom);

#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated RandomNode with the given name, children,
	 * priority function, and random type.
	 *
	 * @param name		The name of the random node.
	 * @param priority	The priority function of the random node.
	 * @param children 	The children of the random node.
	 * @param preempt	Whether random nodes can be preempted.
	 * @param uniformRandom	Whether children are chosen uniformly at random.
	 *
	 * @return a newly allocated RandomNode with the given name, children,
	 * priority function, and random type.
	 */
	static std::shared_ptr<RandomNode> alloc(const std::string& name,
											 const std::function<float()> priority,
											 const std::vector<std::shared_ptr<BehaviorNode>>& children,
											 bool preempt, bool uniformRandom) {
		std::shared_ptr<RandomNode> result = std::make_shared<RandomNode>();
		return (result->init(name, priority, children, preempt, uniformRandom) ? result : nullptr);
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
#pragma mark Behavior Tree
	/**
	 * Returns whether this node chooses a child to execute uniformly at
	 * random.
	 *
	 * If true, then this node chooses its child uniformly at random. Otherwise,
	 * this node uses a weighted probability among its children based on each
	 * child's priority value. 
	 *
	 * @return true if this node chooses a child to execute uniformly at
	 * random.
	 */
	bool isUniformRandom() const { return _uniformRandom; }

	/**
	 * Updates the priority value for this node and all children beneath it,
	 * running the priority function provided or default priority function
	 * if available for the class.
	 *
	 * This node will set its priority to the average priority of its children
	 * if a priority function is not provided.
	 */
	void updatePriority() override;

#pragma mark -
#pragma mark Internal Helpers
	/**
	 * Returns the child choosen by this random node.
	 *
	 * If the _uniformRandom flag has been set, then this node will choose
	 * among its children uniformly at random. Otherwise, this node will
	 * choose among its children with each child's probability of being
	 * selected weighted by that child's priority value.
	 *
	 * @return the child choosen by this random node.
	 */
	const std::shared_ptr<BehaviorNode>& getChosenChild() const override;
};


}
#endif /* __CU_RANDOM_NODE_H__ */
