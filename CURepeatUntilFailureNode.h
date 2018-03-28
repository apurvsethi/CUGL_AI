//
//  CURepeatUntilFailureNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a repeat until failure decorator behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_REPEAT_UNTIL_FAILURE_NODE_H__
#define __CU_REPEAT_UNTIL_FAILURE_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CURepeaterNode.h>

namespace cugl {
	
/**
 * This class provides a repeat until failure decorator node for a behavior tree.
 *
 * A repeat until failure node is a decorator node which is designed to run its
 * child node repeatedly until the child node's state is failure.
 * The limit on the number of times the node is run can be provided.
 *
 * A repeater node's priority is directly based upon the child node's priority.
 */
class RepeatUntilFailureNode : public RepeaterNode {
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized repeat until failure node.
	 *
	 * You must initialize this RepeatUntillFailureNode before use.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	RepeatUntilFailureNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~RepeatUntilFailureNode() { dispose(); }
	
#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Returns the BehaviorNode::State of the repeat until failure node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * repeat until failure node (and all nodes below this node in the tree).
	 * The state for this node is derived from the state of its child node.
	 *
	 * The priority value of the node is updated within this function, based
	 * on the priority values of the child node.
	 *
	 * @return the BehaviorNode::State of the child node.
	 */
	BehaviorNode::State update() override;
};
	
	
}
#endif /* __CU_REPEAT_UNTIL_FAILURE_NODE_H__ */

