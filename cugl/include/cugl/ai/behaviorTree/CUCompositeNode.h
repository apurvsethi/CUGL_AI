//
//  CUCompositeNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a composite behavior node.
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
//  Version: 5/15/2018
//

#ifndef __CU_COMPOSITE_NODE_H__
#define __CU_COMPOSITE_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUBehaviorNode.h>

namespace cugl {

/**
 * This class provides a composite node for a behavior tree.
 *
 * A composite node is a node that has multiple children. When a composite node
 * starts, it chooses a child to run in some order specified by its subclasses.
 * The composite node can be set to interrupt its running child and choose a
 * new child to run. If it is not set to preempt, the child will continue
 * running until it has either finished running, or the composite node itself
 * is interrupted by its parent. If a child successfully finished running, the
 * composite node will return.
 *
 * A composite node can be provided with a priority function to call when
 * updating its own priority. If a function is not provided, the composite node
 * will set its priority to using a default algorithm, which is specified by
 * its subclasses.
 *
 * This class should not be instantiated directly. Instead, you should use
 * one of its subclasses ({@link PriorityNode}, {@link SelectorNode},
 * {@link RandomNode}).
 */
class CompositeNode : public BehaviorNode {
#pragma mark Values
protected:
	/**
	 * Whether to allow preemption among this nodes children.
	 *
	 * If preemption is allowed, this node may choose a new child not to run
	 * during an update, possibly interrupting an old child node if a different
	 * new child is chosen. Otherwise, the composite node cannot interrupt its
	 * running child to select another child to run.
	 */
	bool _preempt;

	/** The index of the child running (-1 if no child is currently running). */
	int _activeChildPos;

#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized composite node.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	CompositeNode() : BehaviorNode(), _activeChildPos(-1) {};

	/**
	 * Deletes this node, disposing all resources.
	 */
	~CompositeNode() { dispose(); }

	/**
	 * Disposes all of the resources used by this node.
	 *
	 * A disposed CompositeNode can be safely reinitialized. Any children owned
	 * by this node will be released. They will be deleted if no other object
	 * owns them.
	 *
	 * It is unsafe to call this on a composite node that is still currently
	 * inside of a running behavior tree.
	 */
	virtual void dispose() override;

	/**
	 * Initializes a composite node with the given name, type, children, and
	 * priority function.
	 *
	 * @param name		The name of the composite node
	 * @param priority	The priority function of the composite node
	 * @param children 	The children of the composite node
	 * @param preempt	Whether child nodes can be preempted
	 *
	 * @return true if initialization was successful
	 */
	virtual bool init(const std::string& name,
					  const std::function<float()> priority,
					  const std::vector<std::shared_ptr<BehaviorNode>>& children,
					  bool preempt);

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
	 * Returns the number of children of this composite node.
	 *
	 * @return The number of children of this composite node.
	 */
	size_t getChildCount() const { return _children.size(); }

	/**
	 * Returns a (weak) pointer to the child at the given position.
	 *
	 * While children are enumerated in the order by which they were added,
	 * it is recommended to attempt to retrieve a child by name instead.
	 *
	 * As a weak reference, this composite node does not pass ownership of its
	 * child.
	 *
	 * @param pos	The child position.
	 *
	 * @return a (weak) pointer to the child at the given position.
	 */
	 const BehaviorNode* getChild(unsigned int pos) const;

	/**
	 * Returns a (weak) the child at the given position, typecast to a const T
	 * pointer.
	 *
	 * This method is provided to simplify the polymorphism of a behavior tree.
	 * While all children are a subclass of type BehaviorNode, you may want to
	 * access them by their specific subclass.  If the child is not an instance
	 * of type T (or a subclass), this method returns nullptr.
	 *
	 * While children are enumerated in the order by which they were added,
	 * it is recommended to attempt to retrieve a child by name instead.
	 *
	 * As a weak reference, this composite node does not pass ownership of its
	 * child.
	 *
	 * @param pos	The child position.
	 *
	 * @return a (weak) reference to the child at the given position, typecast
	 * to a const T pointer.
	 */
	template <typename T>
	inline const T* getChild(unsigned int pos) const {
		return dynamic_cast<const T*>(getChild(pos));
	}

	/**
	 * Returns a (weak) reference the (first) child with the given name.
	 *
	 * If there is more than one child of the given name, it returns the first
	 * one that is found.
	 *
	 * As a weak reference, this composite node does not pass ownership of its
	 * child. In addition, the value may be a nullptr.
	 *
	 * @param name	An identifier to find the child node.
	 *
	 * @return a (weak) reference to the (first) child with the given name.
	 */
	const BehaviorNode* getChildByName(const std::string& name) const;

	/**
	 * Returns a (weak) the (first) child with the given name, typecast to a
	 * const T pointer.
	 *
	 * This method is provided to simplify the polymorphism of a behavior tree.
	 * While all children are a subclass of type BehaviorNode, you may want to
	 * access them by their specific subclass.  If the child is not an instance
	 * of type T (or a subclass), this method returns nullptr.
	 *
	 * If there is more than one child of the given name, it returns the first
	 * one that is found.
	 *
	 * As a weak reference, this composite node does not pass ownership of its
	 * child. In addition, the value may be a nullptr.
	 *
	 * @param name	An identifier to find the child node.
	 *
	 * @return a (weak) reference to the (first) child with the given name,
	 * typecast to a const T pointer.
	 */
	template <typename T>
	inline const T* getChildByName(const std::string& name) const {
		return dynamic_cast<const T*>(getChildByName(name));
	}

	/**
	 * Returns a (weak) reference to the child with the given priority index.
	 *
	 * A child with a specific priority index i is the child with the ith
	 * highest priority. Ties are broken arbitrarily.
	 *
	 * As a weak reference, this composite node does not pass ownership of its
	 * child.
	 *
	 * @param index	The child's priority index.
	 *
	 * @return a (weak) reference the child with the given priority index.
	 */
	const BehaviorNode* getChildByPriorityIndex(unsigned int index) const;

	/**
	 * Returns a (weak) reference to the child with the given priority index,
	 * typecast to a const T pointer.
	 *
	 * This method is provided to simplify the polymorphism of a behavior tree.
	 * While all children are a subclass of type BehaviorNode, you may want to
	 * access them by their specific subclass.  If the child is not an instance
	 * of type T (or a subclass), this method returns nullptr.
	 *
	 * If there is more than one child with the same priority value, the tie is
	 * broken by the child's position in the parent list.
	 *
	 * As a weak reference, this composite node does not pass ownership of its
	 * child.
	 *
	 * @param index	The child's priority index.
	 *
	 * @return a (weak) reference the child with the given priority index,
	 * typecast to a const T pointer.
	 */
	template <typename T>
	const T* getChildByPriorityIndex(unsigned int index) const {
		return dynamic_cast<const T*>(getChildByPriorityIndex(index));
	}

	/**
	 * Returns a (weak) reference to the child currently running.
	 *
	 * As a weak reference, this composite node does not pass ownership of its
	 * child.
	 *
	 * @return a (weak) reference to the currently active child.
	 */
	const BehaviorNode* getActiveChild() const;

	/**
	 * Returns a (weak) reference to the currently child running, typecast to a
	 * const T pointer.
	 *
	 * This method is provided to simplify the polymorphism of a behavior tree.
	 * While all children are a subclass of type BehaviorNode, you may want to
	 * access them by their specific subclass.  If the child is not an instance
	 * of type T (or a subclass), this method returns nullptr.
	 *
	 * As a weak reference, this composite node does not pass ownership of its
	 * child.
	 *
	 * @return a (weak) reference to the currently active child, typecast as a
	 * const T pointer.
	 */
	template <typename T>
	const T* getActiveChild() const {
		return dynamic_cast<const T*>(getActiveChild());
	}

	/**
	 * Returns the list of (weak) references to the node's children.
	 *
	 * As weak references, this composite node does not pass ownership of its
	 * children. 
	 *
	 * @return the list of (weak) references the node's children.
	 */
	std::vector<const BehaviorNode*> getChildren() const;

	/**
	 * Updates the priority value for this node and all children beneath it,
	 * running the priority function provided or default priority function
	 * if available for the class.
	 */
	virtual void updatePriority() override;

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
	 */
	void preempt() override;

#pragma mark -
#pragma mark Internal Helpers
protected:
	/**
	 * Returns the child choosen by this composite node.
	 *
	 * The algorithm for choosing the child of this node is implementation
	 * specific to the subclasses of this node.
	 *
	 * @return the child choosen by this composite node.
	 */
	virtual const std::shared_ptr<BehaviorNode>& getChosenChild() const = 0;
};


}
#endif /* __CU_COMPOSITE_NODE_H__ */
