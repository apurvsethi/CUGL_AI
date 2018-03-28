//
//  CUBehaviorNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a behavior node as part of a
//  behavior tree.
//
//  You should never instantiate an object of this class.  Instead, you should
//  use one of the concrete subclasses of BehaviorNode. Because this is an
//  abstract class, it has no allocators.  It only has an initializer.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_BEHAVIOR_NODE_H__
#define __CU_BEHAVIOR_NODE_H__

#include <string>

namespace cugl {
	
/**
 * This class provides a behavior node for a behavior tree.
 *
 * A behavior node refers to any given node within a beahvior tree,
 * whether it is a CompositeNode, DecoratorNode, or LeafNode.
 *
 * A behavior tree is a construction of behavior nodes, with the top
 * node that does not have a parent referring to the root of the tree.
 * The tree must use an update function to run on each tick, updating
 * the state of each node.
 *
 * The root node of a behavior tree returns the state of the currently
 * running node, or the node that ran during the update cycle.
 */
class BehaviorNode {
#pragma mark Values
public:
	/** The current state of the node. */
	enum class State : unsigned int {
		/** The node is finished with an action, which has succeeded. */
		SUCCESS = 0,
		/** The node is finished with an action, which has failed. */
		FAILURE = 1,
		/** The node is currently running and has yet to succeed or fail. */
		RUNNING = 2,
		/** The node has not yet been run. */
		UNINITIALIZED = 3
	};

protected:
	/** The descriptive, identifying name of the node. */
	std::string _name;
	
	/** A weaker pointer to the parent (or null if root). */
	BehaviorNode* _parent;
	
	/**
	 * The current priority, or relevance of this node.
	 *
	 * This should be a value between 0 and 1, and be updated when the
	 * update() function runs for any given behavior node.
	 */
	float _priority;
	
	/** The current state of this node. */
	BehaviorNode::State _state;
	
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized node.
	 *
	 * This constructor should never be called directly, as this is an abstract
	 * class.
	 */
	BehaviorNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~BehaviorNode() { dispose(); }
	
	/**
	 * Disposes all of the resources used by this node.
	 *
	 * A disposed BehaviorNode can be safely reinitialized.
	 *
	 * It is unsafe to call this on a BehaviorNode that is still currently
	 * inside of a running behavior tree.
	 */
	virtual void dispose();
	
	/**
	 * Initializes a node with the given name.
	 *
	 * @param name  The name of the behavior node.
	 *
	 * @return true if initialization was successful.
	 */
	virtual bool init(const std::string& name);
	
#pragma mark -
#pragma mark Identifiers
	/**
	 * Returns a string that is used to identify the node.
	 *
	 * This name is used to identify nodes in the behavior tree.
	 *
	 * @return a string that is used to identify the node.
	 */
	const std::string& getName() const { return _name; }
	
	/**
	 * Sets a string that is used to identify the node.
	 *
	 * This name is used to identify nodes in the behavior tree.
	 *
	 * @param name  A string that is used to identify the node.
	 */
	void setName(const std::string& name) { _name = name; }
	
	/**
	 * Returns a string representation of this node for debugging purposes.
	 *
	 * If verbose is true, the string will include class information.  This
	 * allows us to unambiguously identify the class.
	 *
	 * @param verbose Whether to include class information
	 *
	 * @return a string representation of this node for debuggging purposes.
	 */
	virtual std::string toString(bool verbose = false) const;
	
	/** Cast from a BehaviorNode to a string. */
	operator std::string() const { return toString(); }
	
#pragma mark -
#pragma mark Behavior Trees
	/**
	 * Returns a float that signifies the priority of the behavior node.
	 *
	 * This priority value is used to determine the relevance of a node in
	 * comparison to other nodes. This value is between 0 and 1.
	 *
	 * @return a float that signifies the priority of the behavior node.
	 */
	float getPriority() const { return _priority; }
	
	/**
	 * Returns a BehaviorNode::State that represents the node state.
	 *
	 * This state is used to identify the state of the node. If the node
	 * has no parent, then this is the state of the behavior tree.
	 *
	 * @return a BehaviorNode::State that represents the node state.
	 */
	BehaviorNode::State getState() const { return _state; }
	
	/**
	 * Returns a (weak) pointer to the parent node.
	 *
	 * The purpose of this pointer is to climb back up the behavior tree.
	 * No child asserts ownership of its parent.
	 *
	 * @return a (weak) pointer to the parent node.
	 */
	BehaviorNode* getParent() { return _parent; }
	
	/**
	 * Returns a (weak) pointer to the parent node.
	 *
	 * The purpose of this pointer is to climb back up the behavior tree.
	 * No child asserts ownership of its parent.
	 *
	 * @return a (weak) pointer to the parent node.
	 */
	const BehaviorNode* getParent() const { return _parent; }
	
	/**
	 * Removes this node from its parent node.
	 *
	 * If the node has no parent, nothing happens.
	 */
	void removeFromParent();
	
	/**
	 * Returns the BehaviorNode::State of the composite node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * behavior node (and all nodes below this node in the tree).
	 *
	 * The priority value of the node is updated within this function, based
	 * on either a priority function provided to the node (in the case of a
	 * LeafNode), or the priority values of the nodes below the given node.
	 *
	 * @return the BehaviorNode::State of the behavior node.
	 */
	virtual BehaviorNode::State update() = 0;
	
private:
#pragma mark -
#pragma mark Internal Helpers
	/**
	 * Sets the parent node.
	 *
	 * The purpose of this pointer is to climb back up the scene graph tree.
	 * No child asserts ownership of its parent.
	 *
	 * @param parent    A pointer to the parent node.
	 */
	void setParent(BehaviorNode* parent) { _parent = parent; }
};
	
	
}
#endif /* __CU_BEHAVIOR_NODE_H__ */
