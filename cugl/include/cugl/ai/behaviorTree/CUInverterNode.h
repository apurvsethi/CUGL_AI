//
//  CUInverterNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for an inverter decorator behavior node.
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
//  Version: 5/16/2018
//

#ifndef __CU_INVERTER_NODE_H__
#define __CU_INVERTER_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUDecoratorNode.h>

namespace cugl {

/**
 * This class provides an inverter decorator node for a behavior tree.
 *
 * An inverter node is a decorator node which is designed to run the node below
 * it and set its priority equal to the opposite of its child's priority. As
 * the priority values for behavior tree nodes are between 0 to 1, the priority
 * of this node is 1 - the child's priority value.
 *
 * An inverter node's state is directly based on its child's state. When an
 * inverter node starts, it immediately starts its child. When the child
 * finishes execution, the inverter node also finishes execution.
 */
class InverterNode : public DecoratorNode {
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized inverter node.
	 *
	 * You must initialize this InverterNode before use.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	InverterNode() {};

	/**
	 * Deletes this node, disposing all resources.
	 */
	~InverterNode() { dispose(); }

#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated InverterNode with the given name and child.
	 *
	 * @param name  The name of the inverter node.
	 * @param child The child of the inverter node.
	 *
	 * @return a newly allocated InverterNode with the given name and child.
	 */
	static std::shared_ptr<InverterNode> alloc(const std::string& name,
											   const std::shared_ptr<BehaviorNode>& child) {
		std::shared_ptr<InverterNode> result = std::make_shared<InverterNode>();
		return (result->init(name, child) ? result : nullptr);
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
	 * Updates the priority value for this node and all children beneath it,
	 * running the priority function provided or default priority function
	 * if available for the class.
	 */
	void updatePriority() override {
		_children[0]->updatePriority();
		_priority = 1 - _children[0]->getPriority();
	}
};


}
#endif /* __CU_INVERTER_NODE_H__ */
