//
//  CUTimerNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a decorator behavior node with a timed
//  delay.
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

#ifndef __CU_TIMER_NODE_H__
#define __CU_TIMER_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUDecoratorNode.h>

namespace cugl {

/**
 * This class provides a decorator node for a behavior tree with a timed delay.
 *
 * A timer node is a decorator node that can be specified to either delay the
 * execution of its child for a given amount of time, or ensure that its child
 * cannot be run again for a given amount of time after being preempted by its
 * parent.
 *
 * The priority of a timer decorator node that is stopping execution of its
 * child after preemption is set to 0. Otherwise, its priority will be assigned
 * as the priority of its child.
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
	 */
	bool _timeDelay;

	/**
	 * The amount of time that the TimerNode will delay execution of its child
	 * or prevent its child from running after preemption.
	 */
	float _delay;

	/**
	 * Whether this node is currently acting to delay execution of its child
	 * or prevent its child from running after preemption.
	 */
	bool _delaying;

	/**
	 * The current amount of time for which the TimerNode has delayed execution
	 * of its child or prevented its child from running after preemption.
	 */
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
	TimerNode() {};

	/**
	 * Deletes this node, disposing all resources.
	 */
	~TimerNode() { dispose(); }

	/**
	 * Disposes all of the resources used by this node.
	 *
	 * A disposed TimerNode can be safely reinitialized. This node's child will
	 * be released, and will be deleted if no other object owns them.
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
			  const std::shared_ptr<BehaviorNode>& child) override {
		return init(name, child, 1, true);
	}

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
	 * Returns the type of delay (before or after). True if before, otherwise
	 * false.
	 *
	 * @return the type of delay (before or after). True if before, otherwise
	 * false.
	 */
	bool getDelayType() const { return _timeDelay; }

	/**
	 * Returns the number of seconds that the TimerNode will delay execution of
	 * its child or prevent its child from running after preemption.
	 *
	 * @return the number of seconds that the TimerNode will delay execution of
	 * its child or prevent its child from running after preemption.
	 */
	float getDelay() const { return _delay; }

	/**
	 * Returns current amount of time for which the TimerNode has delayed
	 * execution of its child or prevented its child from running after
	 * preemption.
	 *
	 * @param The current amount of time for which the TimerNode has delayed
	 * execution of its child or prevented its child from running after
	 * preemption.
	 */
	float getCurrentDelay() const { return _currentDelay; }

	/**
	 * Sets the state of this node.
	 *
	 * This state is used to identify the state of the node. If the node
	 * has no parent, then this is the state of the behavior tree.
	 *
	 * @param state The state of this node.
	 */
	void setState(BehaviorNode::State state) override;

	/**
	 * Reset this node and all nodes below it to an uninitialized state. Also
	 * resets any class values to those set at the start of the tree.
	 */
	void reset() override;

	/**
	 * Updates the priority value for this node and all children beneath it,
	 * running the priority function provided or default priority function
	 * if available for the class.
	 */
	void updatePriority() override;

	/**
	 * Updates this node and any nodes under it.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * behavior node (and nodes below it in the tree).
	 *
	 * Update priority may be run as part of this function, based on whether a
	 * composite node uses preemption.
	 *
	 * @param dt	The elapsed time since the last frame.
	 *
	 * @return the state of this node after updating.
	 */
	BehaviorNode::State update(float dt) override;

	/**
	 * Stops this node from running, and also stops any running nodes under
	 * this node in the tree if they exist.
	 *
	 * If this node is specified to delay after preemption, then this method
	 * will begin the delay.
	 */
	void preempt() override;
};


}
#endif /* __CU_TIMER_NODE_H__ */
