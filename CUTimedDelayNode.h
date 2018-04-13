//
//  CURepeaterNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a decorator behavior node with a timed
//  delay.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_TIMED_DELAY_NODE_H__
#define __CU_TIMED_DELAY_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUDecoratorNode.h>

namespace cugl {
	
/**
 * This class provides a decorator node with a timed delay for a behavior tree.
 *
 * A timed delay decorator node will delay the execution of its
 * child for a given amount of time. Until that time occurs, the priority of this
 * node is 0.
 */
class TimedDelayNode : public DecoratorNode {
#pragma mark Values
protected:
	/** The delay before beginning executing in seconds. */
	float _delay;

	/** The current time that has been delayed. */
	float _current_delay;
	
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized TimedDelayNode.
	 *
	 * You must initialize this TimedDelayNode before use.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	TimedDelayNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~TimedDelayNode() { dispose(); }
	

	/**
	 * Initializes a TimedDelayNode node with the given name and.
	 *
	 * @param name  The name of the timed delay node.
	 *
	 * @return true if initialization was successful.
	 */
	bool init(const std::string& name) {
		return init(name);
	}
	
	/**
	 * Initializes a TimedDelay node with a given name and delay.
	 * 
	 * @param name  The name of the timed delay node.
	 * @param delay The limit on the number of times the child node runs.
	 *
	 * @return true if initialization was successful.
	 */
	bool initWithDelay(const std::string& name, float delay) {
		setDelay(delay);
		return init(name);
	}

	/**
	 * Initializes a TimedDelayNode node with the given name, limit, priority, 
	 * and child.
	 *
	 * @param name  The name of the timed delay node.
	 * @param limit The limit on the number of times the child node runs.
	 * @param child The child of the timed dealy node.
	 * @param priority The priority function for the timed delay node.
	 *
	 * @return true if initialization was successful.
	 */
	bool initWithData(const std::string& name, float delay,
					  const std::shared_ptr<BehaviorNode>& child,
					  const std::function<float()>& priority) {
		setDelay(delay);
		setChild(child);
		setPriorityFunction(priority);
		return init(name);
	}
	
#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated TimedDelayNode with the given name.
	 *
	 * @param name  The name of the timed delay node.
	 *
	 * @return a newly allocated TimedDelayNode with the given name.
	 */
	static std::shared_ptr<TimedDelayNode> alloc(const std::string& name) {
		std::shared_ptr <TimedDelayNode> result = std::make_shared <TimedDelayNode>();
		return (result->init(name) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated TimedDelayNode with the given name and child.
	 *
	 * @param name  The name of the timed delay node.
	 * @param child The child of the timed delay node.
	 *
	 * @return a newly allocated TimedDelayNode with the given name and child.
	 */
	static std::shared_ptr <TimedDelayNode> allocWithChild(const std::string& name,
														const std::shared_ptr<BehaviorNode>& child) {
		std::shared_ptr <TimedDelayNode> result = std::make_shared <TimedDelayNode>();
		return (result->initWithChild(name, child) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated TimedDelayNode with the given name and delay.
	 *
	 * @param name  The name of the timed delay node.
	 * @param delay The delay before the child will begin running.
	 *
	 * @return a newly allocated TimedDelayNode with the given name and delay.
	 */
	static std::shared_ptr <TimedDelayNode> allocWithDelay(const std::string& name, float delay) {
		std::shared_ptr <TimedDelayNode> result = std::make_shared <TimedDelayNode>();
		return (result->initWithDelay(name, delay) ? result : nullptr);
	}

	/**
	 * Returns a newly allocated TimedDelayNode with the given name and priority 
	 * function.
	 * 
	 * @param name The name of the timed delay node.
	 * @param priority The priority of the timed delay node.
	 * 
	 * @return a newly allocated TimedDelayNode with the given name and priorty function.
	 */
	static std::shared_ptr <TimedDelayNode> allocWithPriority(const std::string& name, 
														      const std::function<float()>& priority) {
		std::shared_ptr <TimedDelayNode> result = std::make_shared <TimedDelayNode>();
		return (result->initWithLimit(name, priority) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated TimedDelayNode with the given name, limit, child, and priority function.
	 *
	 * @param name  The name of the repeater node.
	 * @param limit The limit on the number of times the child node runs.
	 * @param child The child of the repeater node.
	 * @param priority The priority function of the repeater node.
	 *
	 * @return a newly allocated TimedDelayNode with the given name and child.
	 */
	static std::shared_ptr <TimedDelayNode> allocWithData(const std::string& name, float limit,
													      const std::shared_ptr<BehaviorNode>& child,
														  const std::function<float()>& priority) {
		std::shared_ptr <TimedDelayNode> result = std::make_shared <TimedDelayNode>();
		return (result->initWithData(name, limit, child, priority) ? result : nullptr);
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
	 * Sets the number of seconds before the child node begins running.
	 *
	 * The delay will prevent the child from returning for a certain
	 * period of time.
	 *
	 * @param delay	the number of seconds before the child node begins running.
	 */
	void setDelay(float delay);
	
	/**
	 * Returns the BehaviorNode::State of the timed delay node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * timed delay node (and all nodes below this node in the tree).
	 * The state for this node is derived from the state of its child node.
	 *
	 * The priority value of the node is updated within this function or
	 * based on the priority values of the child node if no priority function
	 * has been provided.
	 *
	 * @param dt The elapsed time since the last frame.
	 * 
	 * @return the BehaviorNode::State of the child node.
	 */
	BehaviorNode::State update(float dt) override;
};
	
	
}
#endif /* __CU_TIMED_DELAY_NODE_H__ */
