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

	/**
	 * Disposes all of the resources used by this node.
	 *
	 * A disposed RandomNode can be safely reinitialized.
	 *
	 * It is unsafe to call this on a RandomNode that is still currently
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
	 * @param name		The name of the composite node.
	 * @param priority	The priority function of the composite node.
	 * @param children 	The children of the composite node.
	 * @param preempt	Whether child nodes can be preempted.
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
	virtual std::string toString(bool verbose = false) const override;

#pragma mark -
#pragma mark Internal Helpers
	/**
	* Returns the child choosen by this composite node.
	*
	* The algorithm for choosing the child of this node is implementation
	* specific to the subclasses of this node.
	*
	* @return the child choosen by this composite node.
	*/
	virtual const std::shared_ptr<BehaviorNode>& getChosenChild() const override;

};


}
#endif /* __CU_RANDOM_NODE_H__ */
