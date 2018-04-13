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
 * This class provides a decorator node  with a timed delay for a behavior tree.
 *
 * .
 *
 * A repeater node's priority is directly based upon the child node's priority.
 */
class TimedDelayNode : public DecoratorNode {
#pragma mark Values
protected:
	/** The number of times the child has run. */
	unsigned int _numRuns;
	
	/** The limit on the number of times the child node runs. */
	unsigned int _limit;
	
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
	RepeaterNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~RepeaterNode() { dispose(); }
	
	/**
	 * Initializes a TimedDelayNode node with the given name and limit.
	 *
	 * @param name  The name of the timed delay node.
	 * @param limit The limit on the number of times the child node runs.
	 *
	 * @return true if initialization was successful.
	 */
	bool initWithLimit(const std::string& name, unsigned int limit) {
		setLimit(limit);
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
	bool initWithData(const std::string& name, unsigned int limit,
					  const std::shared_ptr<BehaviorNode>& child,
					  const std::function<float()>& priority) {
		setLimit(limit);
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
	 * Returns a newly allocated TimedDelayNode with the given name and limit.
	 *
	 * @param name  The name of the timed delay node.
	 * @param limit The limit on the number of times the child node runs.
	 *
	 * @return a newly allocated TimedDelayNode with the given name and limit.
	 */
	static std::shared_ptr <TimedDelayNode> allocWithLimit(const std::string& name, unsigned limit) {
		std::shared_ptr <TimedDelayNode> result = std::make_shared <TimedDelayNode>();
		return (result->initWithLimit(name, limit) ? result : nullptr);
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
	static std::shared_ptr <TimedDelayNode> allocWithLimit(const std::string& name, 
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
	 * Returns the number of times the child has run.
	 *
	 * @return the number of times the child has run.
	 */
	float getNumRuns() const { return _numRuns; }
	
	/**
	 * Returns the limit on the number of times the child node can run.
	 *
	 * This limit is used to limit the maximum number of times a child node
	 * can run.
	 *
	 * @return the limit on the number of times the child node can run.
	 */
	float getLimit() const { return _limit; }
	
	/**
	 * Sets the limit on the number of times the child node can run if this
	 * node is not currently active.
	 *
	 * This limit is used to limit the maximum number of times a child node
	 * can run.
	 *
	 * @param limit	A the limit on the number of times the child node can run.
	 */
	void setLimit(float limit);
	
	/**
	 * Returns the BehaviorNode::State of the repeater node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * repeater node (and all nodes below this node in the tree).
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
#endif /* __CU_REPEATER_NODE_H__ */
