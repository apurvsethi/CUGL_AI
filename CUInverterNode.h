//
//  CUInverterNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for an inverter decorator behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
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
 * it and set its state to the opposite of the state of its child node (when the
 * child node has finished running.
 *
 * An inverter node's priority is directly based upon the child node's priority.
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
	InverterNode();

	/**
	 * Deletes this node, disposing all resources.
	 */
	~InverterNode() { dispose(); }

#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated InverterNode with the given name and child.
	 *
	 * @param name  The name of the decorator node.
	 * @param child The child of the decorator node.
	 *
	 * @return a newly allocated InverterNode with the given name and child.
	 */
	static std::shared_ptr<InverterNode> alloc(const std::string& name,
											   const std::shared_ptr<BehaviorNode>& child) {
		std::shared_ptr<InverterNode> result = std::make_shared<InverterNode>();
		return (result->init(name, child) ? result : nullptr);
	}

#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Returns the BehaviorNode::State of the inverter node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * inverter node (and all nodes below this node in the tree).
	 * The state for this node is derived from the state of its child node.
	 *
	 * The priority value of the node is updated within this function, based
	 * on the priority values of the child node.
	 *
	 * @param dt	The elapsed time since the last frame.
	 *
	 * @return the BehaviorNode::State of the child node.
	 */
	BehaviorNode::State update(float dt) override;
};


}
#endif /* __CU_INVERTER_NODE_H__ */
