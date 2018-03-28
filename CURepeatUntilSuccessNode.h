//
//  CURepeatUntilSuccessNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a repeat until success decorator behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_REPEAT_UNTIL_SUCCESS_NODE_H__
#define __CU_REPEAT_UNTIL_SUCCESS_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CURepeaterNode.h>

namespace cugl {
	
/**
 * This class provides a repeat until success decorator node for a behavior tree.
 *
 * A repeat until success node is a decorator node which is designed to run its
 * child node repeatedly until the child node's state is success.
 * The limit on the number of times the node is run can be provided.
 *
 * A repeater node's priority is directly based upon the child node's priority.
 */
class RepeatUntilSuccessNode : public RepeaterNode {
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized repeat until success node.
	 *
	 * You must initialize this RepeatUntillSuccessNode before use.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	RepeatUntilSuccessNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~RepeatUntilSuccessNode() { dispose(); }
	
#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Returns the BehaviorNode::State of the repeat until success node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * repeat until success node (and all nodes below this node in the tree).
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
#endif /* __CU_REPEAT_UNTIL_SUCCESS_NODE_H__ */
