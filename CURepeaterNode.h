//
//  CURepeaterNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a repeater decorator behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_REPEATER_NODE_H__
#define __CU_REPEATER_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUDecoratorNode.h>

namespace cugl {
	
/**
 * This class provides a repeater decorator node for a behavior tree.
 *
 * A repeater node is a decorator node which is designed to run its child node
 * repeatedly. The limit on the number of times the node is run can be provided.
 *
 * A repeater node's priority is directly based upon the child node's priority.
 */
class RepeaterNode : public DecoratorNode {
#pragma mark Values
protected:
	/** The number of times the child has run. */
	float _numRuns;
	
	/** The limit on the number of times the child node runs. */
	float _limit;
	
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized repeater node.
	 *
	 * You must initialize this RepeaterNode before use.
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
	 * Initializes a repeater node with the given name and limit.
	 *
	 * @param name  The name of the repeater node.
	 * @param limit The limit on the number of times the child node runs.
	 *
	 * @return true if initialization was successful.
	 */
	bool initWithLimit(const std::string& name, unsigned int limit) {
		setLimit(limit);
		return init(name);
	}
	
	/**
	 * Initializes a repeater node with the given name, limit, and child.
	 *
	 * @param name  The name of the repeater node.
	 * @param limit The limit on the number of times the child node runs.
	 * @param child The child of the repeater node.
	 *
	 * @return true if initialization was successful.
	 */
	bool initWithData(const std::string& name, unsigned int limit,
					  const std::shared_ptr<BehaviorNode>& child) {
		setLimit(limit);
		setChild(child);
		return init(name);
	}
	
#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated RepeaterNode with the given name.
	 *
	 * @param name  The name of the repeater node.
	 *
	 * @return a newly allocated RepeaterNode with the given name.
	 */
	static std::shared_ptr<RepeaterNode> alloc(const std::string& name) {
		std::shared_ptr<RepeaterNode> result = std::make_shared<RepeaterNode>();
		return (result->init(name) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated RepeaterNode with the given name and child.
	 *
	 * @param name  The name of the repeater node.
	 * @param child The child of the repeater node.
	 *
	 * @return a newly allocated RepeaterNode with the given name and child.
	 */
	static std::shared_ptr<RepeaterNode> allocWithChild(const std::string& name,
														const std::shared_ptr<BehaviorNode>& child) {
		std::shared_ptr<RepeaterNode> result = std::make_shared<RepeaterNode>();
		return (result->initWithChild(name, child) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated RepeaterNode with the given name and child.
	 *
	 * @param name  The name of the repeater node.
	 * @param limit The limit on the number of times the child node runs.
	 *
	 * @return a newly allocated RepeaterNode with the given name and child.
	 */
	static std::shared_ptr<RepeaterNode> allocWithLimit(const std::string& name, float limit) {
		std::shared_ptr<RepeaterNode> result = std::make_shared<RepeaterNode>();
		return (result->initWithLimit(name, limit) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated RepeaterNode with the given name, limit, and child.
	 *
	 * @param name  The name of the repeater node.
	 * @param limit The limit on the number of times the child node runs.
	 * @param child The child of the repeater node.
	 *
	 * @return a newly allocated RepeaterNode with the given name and child.
	 */
	static std::shared_ptr<RepeaterNode> allocWithData(const std::string& name, float limit,
													   const std::shared_ptr<BehaviorNode>& child) {
		std::shared_ptr<RepeaterNode> result = std::make_shared<RepeaterNode>();
		return (result->initWithData(name, limit, child) ? result : nullptr);
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
	 * Sets the limit on the number of times the child node can run.
	 *
	 * This limit is used to limit the maximum number of times a child node
	 * can run.
	 *
	 * @param limit	A the limit on the number of times the child node can run.
	 */
	void setLimit(float limit) { _limit = limit; }
	
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
