//
//  CUBehaviorNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a behavior node as part of a behavior tree.
//  The behavior tree node chooses an action by setting a priority for each node
//  and traversing down the tree to select an action
//
//  As this is an abstract class, it has no static constructors.  However, we
//  still separate initialization from the constructor as with all classes in
//  this engine.
//
//  This class uses our standard shared-pointer architecture.
//
//  Author: Apurv Sethi and Andrew Matsumoto
//  Version: 5/7/2018
//

#ifndef __CU_BEHAVIOR_NODE_H__
#define __CU_BEHAVIOR_NODE_H__

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUBehaviorAction.h>

namespace cugl {

#pragma mark Behavior Node Defintion
/**
 * This stuct is a reusable defintion the necessary information to construct a
 * {@link BehaviorNode}. This definition is used by the {@link BehaviorManager}
 * to create a behavior tree node.
 */
struct BehaviorNodeDef : std::enable_shared_from_this<BehaviorNodeDef> {
	/**
	 * This enum is used to describe the type of the {@link BehaviorNode}.
	 *
	 * When creating an instance of a behavior tree node from a BehaviorNodeDef,
	 * this enum is used to determine the type of behavior tree node created.
	 */
	enum class Type {
		/**
		 * A priority node is a composite node, or a node with one or more
		 * children, that chooses the child with the highest priority to run.
		 */
		PRIORITY_NODE,
		/**
		 * A selector node is a composite node, or a node with one or more
		 * children, that runs the first child in its list of children with a
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

	/** The type of behavior tree node this definition describes. */
	Type _type;

	/**
	 * The priority function for this behavior tree node.
	 *
	 * This function is used to assign a priority to a particular node. This
	 * function must return a value between 0 and 1.
	 *
	 * This function must be provided if this node is a {@link LeafNode} and
	 * is optional if this node is a composite node ({@link PriorityNode},
	 * {@link SelectorNode}, {@link RandomNode}). This function is not used if
	 * this node is a decorator node ({@link InverterNode}, {@link TimerNode}).
	 */
	std::function<float()> _priorityFunc;

	/**
	 * Whether a node should choose a new child node on each update, possibly
	 * interrupting an old child node's execution if a different child is
	 * chosen. If true, the node can interrupt a running child node,
	 * otherwise a choosen running child cannot be interrupted to run a
	 * different child.
	 *
	 * This flag is only used if this node is a composite node
	 * ({@link PriorityNode}, {@link SelectorNode}, {@link RandomNode}).
	 */
	bool _preempt;

	/**
	 * Whether a random node should choose the a child for execution uniformly
	 * at random among its children, or choose a child randomly with each
	 * child's probability of being choosen weighted by its priority. If true,
	 * then the node chooses uniformly at random, otherwise, the node uses a
	 * weighted probability.
	 *
	 * This flag is only used if this node is a {@link RandomNode}.
	 */
	bool _uniformRandom;

	/**
	 * The array of children for this node.
	 *
	 * If this node is a leaf node, then this vector should be empty. If this
	 * node is a decorator node, then this vector should have exactly one
	 * element.
	 */
	std::vector<std::shared_ptr<BehaviorNodeDef>> _children;

	/**
	 * Whether a timer node should delay before execution or after preemption.
	 * If true, then the node will delay before running its child, otherwise,
	 * the node cannot be choosen for a certain amount of time after preemption.
	 *
	 * This flag is only used if this node is a {@link TimerNode}.
	 */
	bool _timeDelay;

	/**
	 * The amount of time to delay in seconds.
	 *
	 * This value is only used if this node is a {@link TimerNode}.
	 */
	float _delay;

	/**
	 * The action performed when this node is run.
	 *
	 * This value is only used when this node is a {@link LeafNode}.
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

#pragma mark -

/**
 * An abstract class for a behavior tree node.
 *
 * This class is a base class for the individual nodes of the behavior tree.
 * Behavior tree nodes are either composite, decorator, or leaf nodes.
 *
 * A behavior tree is a construction of behavior nodes. The top node without
 * a parent is the the root of the tree. The tree chooses the action to run
 * based on the priority value of each of the root's descendents. The tree
 * must use an update function to run on each tick, updating the state of each
 * node. The root node of a behavior tree returns the state of the currently
 * running node or of the node that ran during the behavior tree's execution.
 *
 * This class has abstract methods for calculating the priority and updating,
 * which are implemented by the subclasses.
 *
 * Behavior trees should be managed by a {@link BehaviorManager}, which creates
 * each BehaviorNode from a {@link BehaviorNodeDef} and runs and updates the
 * behavior trees.
 */
class BehaviorNode {
#pragma mark Values
public:
	/** The current state of the node. */
	enum class State : unsigned int {
		/** The node is finished with an action. */
		FINISHED = 0,
		/** The node is currently running. */
		RUNNING = 1,
		/** The node is neither running nor has already finished with an action. */
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
	 * {@link updatePriority()} function runs for any given behavior node.
	 */
	float _priority;

	/**
	 * The current priority function for this behavior node.
	 *
	 * This function should return a value between 0 and 1 representing the
	 * priority. This function can be user defined, and the default methods
	 * for calculating the priority can be defined by the subclasses.
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
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
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
	 * A disposed BehaviorNode can be safely reinitialized. Any children owned
	 * by this node will be released. They will be deleted if no other object
	 * owns them.
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
	 * Returns a float that signifies the priority of this behavior tree node.
	 *
	 * This priority value is used to determine the relevance of a node in
	 * comparison to other nodes. This value is between 0 and 1.
	 *
	 * @return a float that signifies the priority of this behavior tree node.
	 */
	float getPriority() const { return _priority; }

	/**
	 * Returns a BehaviorNode::State that represents this node's state.
	 *
	 * This state is used to identify the state of this node. If this node
	 * has no parent, then this is the state of the behavior tree.
	 *
	 * @return a BehaviorNode::State that represents this node's state.
	 */
	BehaviorNode::State getState() const { return _state; }

	/**
	 * Sets the state of this node.
	 *
	 * This state is used to identify the state of this node. If this node
	 * has no parent, then this is the state of the behavior tree.
	 *
	 * @param state The state of this node.
	 */
	virtual void setState(BehaviorNode::State state) { _state = state; }

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
	 * Removes this node from its parent.
	 *
	 * If this node has no parent, nothing happens.
	 */
	void removeFromParent() { if (_parent) _parent->removeChild(_childOffset); }

	/**
	 * Begin running this node, moving from an uninitialized state to a running
	 * state as the correct action to perform as all priority values are updated
	 * and the correct node to run is found through choosing a leaf node.
	 */
	void start() {
		updatePriority();
		setState(BehaviorNode::State::RUNNING);
		update(0.0f);
	}

	/**
	 * Updates the priority value for this node and all children beneath it,
	 * running the priority function provided or default priority function
	 * if available for the class.
	 */
	virtual void updatePriority() = 0;

	/**
	 * Returns the BehaviorNode::State of this node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * behavior node (and nodes chosen to run below it in the tree).
	 *
	 * Update priority may be run as part of this function, based on whether a
	 * composite node uses preemption.
	 *
	 * @param dt	The elapsed time since the last frame.
	 *
	 * @return the BehaviorNode::State of this behavior tree node.
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
