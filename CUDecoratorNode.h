//
//  CUDecoratorNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a decorator behavior node.
//
//  You should never instantiate an object of this class.  Instead, you should
//  use one of the concrete subclasses of DecoratorNode. Because this is an
//  abstract class, it has no allocators.  It only has an initializer.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_DECORATOR_NODE_H__
#define __CU_DECORATOR_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUBehaviorNode.h>

namespace cugl {

/**
 * This class provides a decorator behavior node for a behavior tree.
 *
 * A decorator node within a behavior tree refers to the set of nodes that have
 * one child, and perform some function regarding the priority value of the
 * child node. There are rules specific to each type of decorator node defining
 * this.
 *
 * The two concrete subclasses for a DecoratorNode are: InverterNode and
 * TimerNode. While similar in structure, each class has key differences
 * defining how it runs in relation to its child
 * node.
 */
class DecoratorNode : public BehaviorNode {
#pragma mark Values
protected:
	/** The child of this decorator node. */
	std::shared_ptr<BehaviorNode> _child;

#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized decorator node.
	 *
	 * This constructor should never be called directly, as this is an abstract
	 * class.
	 */
	DecoratorNode() : BehaviorNode(), _child(nullptr) {};

	/**
	 * Deletes this node, disposing all resources.
	 */
	~DecoratorNode() { dispose(); }

	/**
	 * Disposes all of the resources used by this node, and all descendants
	 * in the tree.
	 *
	 * A disposed DecoratorNode can be safely reinitialized.
	 *
	 * It is unsafe to call this on a DecoratorNode that is still currently
	 * inside of a running behavior tree.
	 */
	virtual void dispose() override;

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
	 * Returns the node's child.
	 *
	 * @return the node's child.
	 */
	const std::shared_ptr<BehaviorNode>& getChild() const { return _child; }

	/**
	 * Returns the node's child, typecast to a shared T pointer.
	 *
	 * This method is provided to simplify the polymorphism of a behavior tree.
	 * While the child is a subclass of type BehaviorNode, you may want to
	 * access them by their specific subclass.  If the child is not an instance
	 * of type T (or a subclass), this method returns nullptr.
	 *
	 * @return the child at the given position, typecast to a shared T pointer.
	 */
	template <typename T>
	inline std::shared_ptr<T> getChild() const {
		return std::dynamic_pointer_cast<T>(getChild());
	}

	/**
	 * Updates the priority value for this node and all children beneath it,
	 * running the piority function provided or default priority function
	 * if available for the class.
	 */
	virtual void updatePriority() override = 0;

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
	BehaviorNode::State update(float dt) override {
		setState(_child->update(dt));
		return getState();
	}

	/**
	 * Stops this node from running, and also stops any running nodes under
	 * this node in the tree if they exist.
	 */
	virtual void preempt() override = 0;

protected:
	/**
	 * Removes the child at the given position from this node.
	 *
	 * @param pos   The position of the child node which will be removed.
	 */
	void removeChild(unsigned int pos) override {
		_child->setParent(nullptr);
		_child->setChildOffset(-1);
		_child = nullptr;
	}
};


}
#endif /* __CU_DECORATOR_NODE_H__ */
