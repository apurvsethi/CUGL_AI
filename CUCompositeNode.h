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
	/** The array of children for this composite node. */
	std::vector<std::shared_ptr<BehaviorNode>> _children;
	
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized composite node.
	 *
     * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
     * the heap, use one of the static constructors instead.
	 */
	CompositeNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~CompositeNode() { dispose(); }
	
	/**
	 * Disposes all of the resources used by this node.
	 *
	 * A disposed CompositeNode can be safely reinitialized.
	 *
	 * It is unsafe to call this on a CompositeNode that is still currently
	 * inside of a running behavior tree.
	 */
	void dispose() override;
	
	/**
	 * Initializes a composite node using the given template def.
	 *
	 * @param behaviorNodeDef	The def specifying arguments for this node.
	 *
	 * @return true if initialization was successful.
	 */
	bool init(const std::shared_ptr<BehaviorNodeDef>& behaviorNodeDef) override;
	
#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Returns the number of children of this composite node.
	 *
	 * @return The number of children of this composite node.
	 */
	size_t getChildCount() const { return _children.size(); }

	/**
	 * Returns the child with the given priority index.
	 *
	 * A child with a specific priority index i is the child with the ith
	 * highest priority. Ties are broken arbitrarily.
	 * 
	 * @param index 	The child's priority index.
	 * 
	 * @return the child with the given priority index.
	 */
	const std::shared_ptr<BehaviorNode>& getChildWithPriorityIndex(unsigned int index) const;
	
	/**
	 * Returns the child at the given position.
	 *
	 * While children are enumerated in the order by which they were added,
	 * it is recommended to attempt to retrieve a child by name instead.
	 *
	 * @param pos   The child position.
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
	 * @param pos   The child position.
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
	 * @param name  An identifier to find the child node.
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
	 * @param name  An identifier to find the child node.
	 *
	 * @return the (first) child with the given name, typecast to a shared T
	 * pointer.
	 */
	template <typename T>
	inline std::shared_ptr<T> getChildByName(const std::string& name) const {
		return std::dynamic_pointer_cast<T>(getChildByName(name));
	}
	
	/**
	 * Returns the list of the node's children.
	 *
	 * @return the list of the node's children.
	 */
	const std::vector<std::shared_ptr<BehaviorNode>>& getChildren() const { return _children; }

	/**
	 * Returns the BehaviorNode::State of the composite node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * composite node (and all nodes below this node in the tree).
	 * The state for this node is derived from the state of the running
	 * or most recently run node.
	 *
	 * The priority value of the node is updated within this function, based
	 * on the priority values of the nodes below the given node.
	 *
	 * @param dt The elapsed time since the last frame.
	 *
	 * @return the BehaviorNode::State of the composite node.
	 */
	virtual BehaviorNode::State update(float dt) override;
};
	
	
}
#endif /* __CU_COMPOSITE_NODE_H__ */
