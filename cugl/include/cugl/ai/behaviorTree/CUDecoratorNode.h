//
//  CUDecoratorNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a decorator behavior node.
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
//  Version: 5/16/2018
//

#ifndef __CU_DECORATOR_NODE_H__
#define __CU_DECORATOR_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUBehaviorNode.h>

namespace cugl {

/**
 * This class provides a decorator node for a behavior tree.
 *
 * A decorator node of a behavior tree has exactly one child and perform some
 * method altering the execution status or returning an altered version of its
 * child's priority. The exact method of performing these modifications is
 * defined within the different subclasses of this node. The status of the
 * decorator node is related to the status of its child.
 *
 * Unlike other types of behavior tree nodes, decorator nodes do not allow a
 * user defined priority function. This is because the decorator node returns
 * either the priority or some modification of the priority of the child.
 *
 * This class should not be instantiated directly. Instead, you should use one
 * of its subclasses ({@link InverterNode} and {@link TimerNode}).
 */
class DecoratorNode : public BehaviorNode {

#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized decorator node.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	DecoratorNode() {};

	/**
	 * Deletes this node, disposing all resources.
	 */
	~DecoratorNode() { dispose(); }

	/**
	 * Initializes a decorator node with the given name and child.
	 *
	 * @param name  The name of the decorator node.
	 * @param child The child of the decorator node.
	 *
	 * @return true if initialization was successful.
	 */
	virtual bool init(const std::string& name, const std::shared_ptr<BehaviorNode>& child);

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
	virtual std::string toString(bool verbose = false) const override = 0;

#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Returns a (weak) reference to this node's child.
	 *
	 * As a weak reference, this decorator node does not pass ownership of its
	 * child.
	 *
	 * @return a (weak) reference to this node's child.
	 */
	const BehaviorNode* getChild() const {
		return _children[0].get();
	}

	/**
	 * Returns a (weak) reference to this node's child, typecast to a shared T
	 * pointer.
	 *
	 * This method is provided to simplify the polymorphism of a behavior tree.
	 * While the child is a subclass of type BehaviorNode, you may want to
	 * access them by their specific subclass.  If the child is not an instance
	 * of type T (or a subclass), this method returns nullptr.
	 *
	 * As a weak reference, this decorator node does not pass ownership of its
	 * child.
	 *
	 * @return a (weak) reference to the child at the given position, typecast to
	 * a shared T pointer.
	 */
	template <typename T>
	inline const T* getChild() const {
		return dynamic_cast<const T*>(getChild());
	}

	/**
	 * Updates the priority value for this node and all children beneath it,
	 * running the priority function provided or default priority function
	 * if available for the class.
	 */
	virtual void updatePriority() override = 0;

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
	virtual BehaviorNode::State update(float dt) override;
};


}
#endif /* __CU_DECORATOR_NODE_H__ */
