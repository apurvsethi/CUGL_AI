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
//  Author: Apurv Sethi and Andrew Matsumoto
//  Version: 3/28/2018
//

#ifndef __CU_BEHAVIOR_NODE_H__
#define __CU_BEHAVIOR_NODE_H__

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUBehaviorAction.h>

namespace cugl {

/**
 * A def through which a behavior node is constructed; a template to use in
 * order to create a node and thus a behavior tree.
 */
struct BehaviorNodeDef : std::enable_shared_from_this<BehaviorNodeDef> {
	/**
	 * This enum is used to determine the type of the BehaviorNode. When
	 * creating an instance of a behavior node from a BehaviorNodeDef, this
	 * enum is used to determine the type of behavior node created.
	 */
	enum class Type {
		/**
		 * A priority node is a composite node, or a node with one or more
		 * children, that uses the priority value of its children to choose the
		 * child that will run.
		 */
		PRIORITY_NODE,
		/**
		 * A selector node is a composite node, or a node with one or more
		 * children, that runs the first child in its list of children with
		 * non-zero priority.
		 */
		SELECTOR_NODE,
		/**
		 * A random node is a composite node, or a node with one or more
		 * children, that runs a child either uniformly at random or uses a
		 * weighted random based on priority values. This choice is based on
		 * the _uniformRandom flag described below.
		 */
		RANDOM_NODE,
		/**
		 * An inverter node is a decorator node, or a node with one child,
		 * that sets its priority value by inverting its child's priority value.
		 * In other words, 1 - priority of child where priority is from 0 to 1.
		 * This node does not use the priority function provided by the user.
		 */
		INVERTER_NODE,
		/**
		 * A timer node is a decorator node, or a node with one child, that
		 * either delays execution of its child node when the child is chosen
		 * for execution by a given time, or ensures that the child is not run
		 * again after its execution for a given time. This choice is based on
		 * the _timeDelay flag described below.
		 */
		TIMER_NODE,
		/**
		 * A leaf node is a node in charge of running an action, and the base
		 * node used for conditional execution (through the priority function).
		 * A leaf node must have an action associated with it, and cannot have
		 * any children.
		 */
		LEAF_NODE
	};

	/** The descriptive, identifying name of the node. */
	std::string _name;

	/** The type of behavior node this def is for. */
	Type _type;

	/**
	 * The priority function for this behavior node.
	 *
	 * This should return a value between 0 and 1 representing the priority.
	 * This function can be user defined, and the default values are defined
	 * by the type of node.
	 */
	std::function<float()> _priorityFunc;

	/**
	 * Whether or not the composite node should choose a new child node on each
	 * execution, possibly interrupting an old child node's execution if a
	 * different node would be chosen now. If true, the composite node can
	 * interrupt a running child node, otherwise a chosen node cannot be
	 * interrupted.
	 *
	 * This flag is only useful for composite nodes (PriorityNode, SelectorNode,
	 * RandomNode).
	 */
	bool _preempt;

	/**
	 * Whether or not the random node should choose the child for execution
	 * based on a uniformly at random choice amongst its children, or should
	 * choose the child randomly with weightage being provided for children
	 * nodes through their priority values. If true, then the node chooses
	 * uniformly at random, otherwise the node uses a weighted probability.
	 *
	 * This flag is only useful for RandomNode.
	 */
	bool _uniformRandom;

	/**
	 * The array of children for this node.
	 *
	 * This should only be used if the node is a composite (PriorityNode,
	 * SelectorNode, RandomNode) or decorator node (Inverter Node, TimerNode)
	 * and cannot be used for a leaf node.
	 *
	 * Additionally, there should only be one child in this vector for decorator
	 * nodes (InverterNode and TimerNode).
	 */
	std::vector<std::shared_ptr<BehaviorNodeDef>> _children;

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

	/**
	 * The delay before beginning executing in seconds.
	 *
	 * This flag is only useful for TimerNode.
	 */
	float _delay;

	/**
	 * The action used when this node is run.
	 *
	 * This should only be used when this node is of type LeafNode.
	 */
	std::shared_ptr<BehaviorAction> _action;

	BehaviorNodeDef() : _type(Type::LEAF_NODE),	_priorityFunc(nullptr),
	_action(nullptr) {}

	/**
	 * Returns the (first) node with the given name found using a recursive
	 * search down from this BehaviorNodeDef.
	 *
	 * If there is more than one node of the given name, it returns the first
	 * one that is found in an unspecified search order. As a result, names
	 * should be unique.
	 *
	 * @param name	An identifier to find the node.
	 *
	 * @return the (first) node with the given name found.
	 */
	std::shared_ptr<BehaviorNodeDef> getNodeByName(const std::string& name);
};

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
		/** The node is finished with an action. */
		FINISHED = 0,
		/** The node is currently running and has yet to succeed or fail. */
		RUNNING = 1,
		/** The node has not yet been run. */
		UNINITIALIZED = 2
	};

protected:
	/** The descriptive, identifying name of the node. */
	std::string _name;

	/** A weaker pointer to the parent (or null if root). */
	BehaviorNode* _parent;

	/** The current state of this node. */
	BehaviorNode::State _state;

	/**
	 * The current priority, or relevance of this node.
	 *
	 * This should be a value between 0 and 1, and be updated when the
	 * update() function runs for any given behavior node.
	 */
	float _priority;

	/**
	 * The current priority function for this behavior node.
	 *
	 * This should return a value between 0 and 1 representing the priority.
	 * This function can be user defined, and the default values can be defined
	 * by the subclasses.
	 */
	std::function<float()> _priorityFunc;

	/** The (current) child offset of this node (-1 if root) */
	int _childOffset;

#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized node.
	 *
	 * This constructor should never be called directly, as this is an abstract
	 * class.
	 */
	BehaviorNode() : _parent(nullptr), _priorityFunc(nullptr),
	_state(BehaviorNode::State::UNINITIALIZED), _childOffset(-2) {}

	/**
	 * Deletes this node, disposing all resources.
	 */
	~BehaviorNode() { dispose(); }

	/**
	 * Disposes all of the resources used by this node, and any descendants
	 * in the tree.
	 *
	 * A disposed BehaviorNode can be safely reinitialized.
	 *
	 * It is unsafe to call this on a BehaviorNode that is still currently
	 * inside of a running behavior tree.
	 */
	virtual void dispose();

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
	 * Returns a string representation of this node for debugging purposes.
	 *
	 * If verbose is true, the string will include class information.  This
	 * allows us to unambiguously identify the class.
	 *
	 * @param verbose	Whether to include class information.
	 *
	 * @return a string representation of this node for debugging purposes.
	 */
	virtual std::string toString(bool verbose = false) const = 0;

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
	 * Sets the state of this node.
	 *
	 * This state is used to identify the state of the node. If the node
	 * has no parent, then this is the state of the behavior tree.
	 *
	 * @param state The state of this node.
	 */
	void setState(BehaviorNode::State state) { _state = state; }

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
	 * Sets the parent of this node.
	 *
	 * The purpose of this pointer is to climb back up the behavior tree.
	 * No child asserts ownership of its parent.
	 *
	 * @param parent The parent of this node.
	 */
	void setParent(BehaviorNode* parent) { _parent = parent; }

	/**
	 * Removes this node from the parent.
	 *
	 * If this node has no parent, nothing happens.
	 */
	void removeFromParent() { if (_parent) _parent->removeChild(_childOffset); }

	/**
	 * Begin running the node, moving from an uninitialized state to a running
	 * state as the correct action to perform as all priority values are updated
	 * and the correct node to run is found through choosing a leaf node.
	 */
	void start() {
		updatePriority();
		update(0.0f);
	}

	/**
	 * Updates the priority value for this node and all children beneath it,
	 * running the priority function provided or default priority function
	 * if available for the class.
	 */
	virtual void updatePriority() = 0;

	/**
	 * Returns the BehaviorNode::State of the node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * behavior node (and nodes chosen to run below it in the tree).
	 *
	 * Update priority may be run as part of this function, based on whether a
	 * composite node uses preemption.
	 *
	 * @param dt	The elapsed time since the last frame.
	 *
	 * @return the BehaviorNode::State of the behavior node.
	 */
	virtual BehaviorNode::State update(float dt) = 0;

	/**
	 * Stops this node from running, and also stops any running nodes under
	 * this node in the tree if they exist.
	 */
	virtual void preempt() = 0;

#pragma mark -
#pragma mark Internal Helpers
	/**
	 * Sets child offset of this node.
	 *
	 * @param offset The child offset of this node.
	 */
	void setChildOffset(int pos) { _childOffset = pos; }

	/**
	 * Gets the child offset of this node.
	 *
	 * @returns The child offset of this node.
	 */
	int getChildOffset() const { return _childOffset; }

protected:
	/**
	 * Returns true if sibling a has a larger priority than sibling b.
	 *
	 * This method is used by std::sort to sort the children. Ties are
	 * broken from the offset of the children.
	 *
	 * @param a The first child
	 * @param b The second child
	 *
	 * @return true if sibling a is has a larger priority than sibling b.
	 */
	static bool compareNodeSibs(const std::shared_ptr<BehaviorNode>& a,
								const std::shared_ptr<BehaviorNode>& b);

	/**
	 * Sets the priority of this node.
	 *
	 * @param priority The priority of this node.
	 */
	void setPriority(float priority);

	/**
	 * Removes the child at the given position from this node.
	 *
	 * @param pos   The position of the child node which will be removed.
	 */
	virtual void removeChild(unsigned int pos) = 0;
};


}
#endif /* __CU_BEHAVIOR_NODE_H__ */
