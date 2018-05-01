//
//  CUCompositeNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a composite behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_COMPOSITE_NODE_H__
#define __CU_COMPOSITE_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUBehaviorNode.h>

namespace cugl {

/**
 * This class provides a composite behavior node for a behavior tree.
 *
 * A composite node within a behavior tree refers to the set of nodes that have
 * multiple children under them and run the children in some order. If a child
 * successfully finishes running, the composite node will return.
 *
 * A composite node can be either a priority node, a selector node, or a random
 * node. A priority node will run in descending order of priority, where higher
 * priority children can interrupt lower priority chidren, until one runs
 * successfully. A selector node will run the children in order until it finds
 * a child with a nonzero priority. A random node will run a random child
 * node, based on either uniform probability or weighted probability.
 */
class CompositeNode : public BehaviorNode {
#pragma mark Values
protected:
	/**
	 * Whether or not the composite node should choose a new child node on each
	 * execution, possibly interrupting an old child node's execution if a
	 * different node would be chosen now. If true, the composite node can
	 * interrupt a running child node, otherwise a chosen node cannot be
	 * interrupted.
	 */
	bool _preempt;

	/** The array of children for this composite node. */
	std::vector<std::shared_ptr<BehaviorNode>> _children;

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
	 * Disposes all of the resources used by this node, and all descendants
	 * in the tree.
	 *
	 * A disposed CompositeNode can be safely reinitialized.
	 *
	 * It is unsafe to call this on a CompositeNode that is still currently
	 * inside of a running behavior tree.
	 */
	virtual void dispose() override;

	/**
	 * Initializes a composite node with the given name, children, and priority
	 * function.
	 *
	 * @param name		The name of the composite node.
	 * @param priority	The priority function of the composite node.
	 * @param children 	The children of the composite node.
	 * @param preempt	Whether child nodes can be preempted.
	 *
	 * @return true if initialization was successful.
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
	 * Returns the child at the given position.
	 *
	 * While children are enumerated in the order by which they were added,
	 * it is recommended to attempt to retrieve a child by name instead.
	 *
	 * @param pos	The child position.
	 *
	 * @return the child at the given position.
	 */
	const std::shared_ptr<BehaviorNode>& getChild(unsigned int pos) const;

	/**
	 * Returns the child at the given position, typecast to a shared T pointer.
	 *
	 * This method is provided to simplify the polymorphism of a behavior tree.
	 * While all children are a subclass of type BehaviorNode, you may want to
	 * access them by their specific subclass.  If the child is not an instance
	 * of type T (or a subclass), this method returns nullptr.
	 *
	 * While children are enumerated in the order by which they were added,
	 * it is recommended to attempt to retrieve a child by name instead.
	 *
	 * @param pos	The child position.
	 *
	 * @return the child at the given position, typecast to a shared T pointer.
	 */
	template <typename T>
	inline std::shared_ptr<T> getChild(unsigned int pos) const {
		return std::dynamic_pointer_cast<T>(getChild(pos));
	}

	/**
	 * Returns the (first) child with the given name.
	 *
	 * If there is more than one child of the given name, it returns the first
	 * one that is found.
	 *
	 * @param name	An identifier to find the child node.
	 *
	 * @return the (first) child with the given name.
	 */
	const std::shared_ptr<BehaviorNode>& getChildByName(const std::string& name) const;

	/**
	 * Returns the (first) child with the given name, typecast to a shared T
	 * pointer.
	 *
	 * This method is provided to simplify the polymorphism of a behavior tree.
	 * While all children are a subclass of type BehaviorNode, you may want to
	 * access them by their specific subclass.  If the child is not an instance
	 * of type T (or a subclass), this method returns nullptr.
	 *
	 * If there is more than one child of the given name, it returns the first
	 * one that is found.
	 *
	 * @param name	An identifier to find the child node.
	 *
	 * @return the (first) child with the given name, typecast to a shared T
	 * pointer.
	 */
	template <typename T>
	inline std::shared_ptr<T> getChildByName(const std::string& name) const {
		return std::dynamic_pointer_cast<T>(getChildByName(name));
	}

	/**
	 * Returns the child with the given priority index.
	 *
	 * A child with a specific priority index i is the child with the ith
	 * highest priority. Ties are broken arbitrarily.
	 *
	 * @param index	The child's priority index.
	 *
	 * @return the child with the given priority index.
	 */
	const std::shared_ptr<BehaviorNode>& getChildByPriorityIndex(unsigned int index) const;

	/**
	 * Returns the child with the given priority index, typecast to a shared T
	 * pointer.
	 *
	 * This method is provided to simplify the polymorphism of a behavior tree.
	 * While all children are a subclass of type BehaviorNode, you may want to
	 * access them by their specific subclass.  If the child is not an instance
	 * of type T (or a subclass), this method returns nullptr.
	 *
	 * If there is more than one child with the same priority value, the tie is
	 * broken arbitrarily.
	 *
	 * @param index	The child's priority index.
	 *
	 * @return the child with the given priority index, typecast to a shared T
	 * pointer.
	 */
	template <typename T>
	inline std::shared_ptr<T> getChildByPriorityIndex(unsigned int index) const {
		return std::dynamic_pointer_cast<T>(getChildByPriorityIndex(index));
	}

	/**
	 * Returns the list of the node's children.
	 *
	 * @return the list of the node's children.
	 */
	const std::vector<std::shared_ptr<BehaviorNode>>& getChildren() const { return _children; }

	/**
	 * Updates the priority value for this node and all children beneath it,
	 * running the piority function provided or default priority function
	 * if available for the class.
	 */
	virtual void updatePriority() override;

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
	virtual BehaviorNode::State update(float dt) override;

	/**
	 * Stops this node from running, and also stops any running nodes under
	 * this node in the tree if they exist.
	 */
	virtual void preempt() override;

#pragma mark -
#pragma mark Internal Helpers
protected:
	/**
	* Removes the child at the given position from this node.
	*
	* @param pos   The position of the child node which will be removed.
	*/
	void removeChild(unsigned int pos) override;

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
