//
//  CUTimerNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a decorator behavior node with a timed
//  delay.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_TIMER_NODE_H__
#define __CU_TIMER_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUDecoratorNode.h>

namespace cugl {
	
/**
 * This class provides a decorator node with a timer for a behavior tree.
 *
 * A timer decorator node will delay the execution of its child for a given
 * amount of time, or ensure that its child cannot be run again for a given
 * amount of time. Until that time occurs, the priority of this node is 0.
 */
class TimerNode : public DecoratorNode {
#pragma mark Values
protected:
	/**
	 * Whether the time provided to the TimerNode should be used to delay
	 * execution of its child node, or should be used to ensure that the child
	 * node is not chosen again for the given amount of time. If true, then
	 * execution is delayed, otherwise the child is not chosen after execution
	 * for the given time.
	 *
	 * This flag is only useful for TimerNode.
	 */
	bool _timeDelay;
	
	/** The delay before beginning executing in seconds. */
	float _delay;

	/** The current time that has been delayed. */
	float _currentDelay;
	
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized TimerNode.
	 *
	 * You must initialize this TimerNode before use.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	TimerNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~TimerNode() { dispose(); }
	
	/**
	 * Disposes all of the resources used by this node.
	 *
	 * A disposed TimerNode can be safely reinitialized.
	 *
	 * It is unsafe to call this on a TimerNode that is still currently
	 * inside of a running behavior tree.
	 */
	void dispose() override;
	
	/**
	 * Initializes a timed delay node with the given name and child. Utilizes
	 * a 1 second delay before running the child node.
	 *
	 * @param name  The name of the timer node.
	 * @param child The child of the timer node.
	 *
	 * @return true if initialization was successful.
	 */
	bool init(const std::string& name,
			  const std::shared_ptr<BehaviorNode>& child) override;

	/**
	 * Initializes a timed delay node with the given name, child, delay type,
	 * and delay.
	 *
	 * @param name  	The name of the timer node.
	 * @param child 	The child of the timer node.
	 * @param timeDelay Whether the child node is delayed before running.
	 * @param delay 	The number of seconds for which the child is delayed.
	 *
	 * @return true if initialization was successful.
	 */
	bool init(const std::string& name,
			  const std::shared_ptr<BehaviorNode>& child,
			  bool timeDelay, float delay);
	
#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated TimerNode with the given name and child.
	 * Utilizes a 1 second delay before running the child node.
	 *
	 * @param name  The name of the timer node.
	 * @param child The child of the timer node.
	 *
	 * @return a newly allocated TimerNode with the given name and child.
	 */
	static std::shared_ptr<TimerNode> alloc(const std::string& name,
											const std::shared_ptr<BehaviorNode>& child) {
		std::shared_ptr<TimerNode> result = std::make_shared<TimerNode>();
		return (result->init(name, child) ? result : nullptr);
	}

	/**
	 * Returns a newly allocated TimerNode with the given name, child, delay
	 * type, and delay.
	 *
	 * @param name  The name of the timer node.
	 * @param child The child of the timer node.
	 * @param timeDelay Whether the child node is delayed before running.
	 * @param delay 	The number of seconds for which the child is delayed.
	 *
	 * @return a newly allocated TimerNode with the given name, child, delay
	 * type, and delay.
	 */
	static std::shared_ptr<TimerNode> alloc(const std::string& name,
											const std::shared_ptr<BehaviorNode>& child,
											bool timeDelay, float delay) {
		std::shared_ptr<TimerNode> result = std::make_shared<TimerNode>();
		return (result->init(name, child, timeDelay, delay) ? result : nullptr);
	}
	
#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Returns the number of seconds before the child node begins running.
	 *
	 * The delay will prevent the child from returning for a certain
	 * period of time.
	 *
	 * @return the number of seconds before the child node begins running.
	 */
	float getDelay() const { return _delay; }
	
	/**
	 * Returns the BehaviorNode::State of the timer node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * timer node (and all nodes below this node in the tree). The state for
	 * his node is derived from the state of its child node.
	 *
	 * The priority value of the node is updated within this function or
	 * based on the priority values of the child node if no priority function
	 * has been provided.
	 *
	 * @param dt	The elapsed time since the last frame.
	 * 
	 * @return the BehaviorNode::State of the child node.
	 */
	BehaviorNode::State update(float dt) override;
};
	
	
}
#endif /* __CU_TIMER_NODE_H__ */
