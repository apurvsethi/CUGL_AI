//
//  CUCompositeNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a composite behavior node.
//
//  You should never instantiate an object of this class.  Instead, you should
//  use one of the concrete subclasses of CompositeNode. Because this is an
//  abstract class, it has no allocators.  It only has an initializer.
//
//  Author: Apurv Sethi
//  Version: 3/27/2018
//

#ifndef __CU_COMPOSITE_NODE_H__
#define __CU_COMPOSITE_NODE_H__

#include <string>
#include <vector>

namespace cugl {
	
/**
 * This class provides a composite behavior node for a behavior tree.
 *
 * A composite node within a behavior tree refers to the set of nodes that have
 * multiple children under them and run the children in some order. There are rules
 * specific to each type of composite node defining how many children are run,
 * the sequence in which they are run, and the definition of sucess or failure.
 *
 * The three concrete subclasses for a CompositeNode are: PriorityNode,
 * SequenceNode, and SelectorNode. While similar in structure, each class has key
 * differences defining how they run in relation to their child nodes.
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
	 * This constructor should never be called directly, as this is an abstract
	 * class.
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
	void dispose();
	
	/**
	 * Initializes a composite node with the given name.
	 *
	 * @param name  The name of the composite node.
	 *
	 * @return true if initialization was successful.
	 */
	bool init(const std::string& name);
	
	/**
	 * Initializes a composite node with the given name and children.
	 *
	 * @param name  The name of the composite node.
	 * @param children The children of the composite node.
	 *
	 * @return true if initialization was successful.
	 */
	bool initWithChildren(const std::string& name,
						  const std::vector<std::shared_ptr<BehaviorNode>>& children);
	
#pragma mark -
#pragma mark Behavior Tree
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
	 * @return the BehaviorNode::State of the composite node.
	 */
	virtual BehaviorNode::State update() = 0;
	
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
	 * @param pos   The child position.
	 *
	 * @return the child at the given position.
	 */
	std::shared_ptr<BehaviorNode> getChild(unsigned int pos);
	
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
	std::shared_ptr<BehaviorNode> getChildByName(const std::string& name) const;
	
	/**
	 * Returns the (first) child with the given name, typecast to a shared T pointer.
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
	 * @return the (first) child with the given name, typecast to a shared T pointer.
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
	std::vector<std::shared_ptr<BehaviorNode>> getChildren() { return _children; }
	
	/**
	 * Returns the list of the node's children.
	 *
	 * @return the list of the node's children.
	 */
	const std::vector<std::shared_ptr<BehaviorNode>>& getChildren() const { return _children; }
	
	/**
	 * Adds a child to this node.
	 *
	 * @param child A child node.
	 */
	void addChild(std::shared_ptr<BehaviorNode> child);
	
	/**
	 * Adds a child to this node with the given name.
	 *
	 * @param child A child node.
	 * @param name  A string to identify the node.
	 */
	void addChildWithName(const std::shared_ptr<BehaviorNode>& child, const std::string &name) {
		addChild(child);
		child->setName(name);
	}
	
	/**
	 * Removes the child at the given position from this CompositeNode.
	 *
	 * Removing a child alters the position of every child after it.  Hence
	 * it is unsafe to cache child positions.
	 *
	 * @param pos   The position of the child node which will be removed.
	 */
	void removeChild(unsigned int pos);
	
	/**
	 * Removes a child from this CompositeNode.
	 *
	 * Removing a child alters the position of every child after it.  Hence
	 * it is unsafe to cache child positions.
	 *
	 * If the child is not in this node, nothing happens.
	 *
	 * @param child The child node which will be removed.
	 */
	void removeChild(const std::shared_ptr<BehaviorNode>& child);
	
	/**
	 * Removes a child from the CompositeNode by name.
	 *
	 * If there is more than one child of the given name, it removes the first
	 * one that is found.
	 *
	 * @param name  A string to identify the node.
	 */
	void removeChildByName(const std::string &name);
	
	/**
	 * Removes all children from this Node.
	 */
	void removeAllChildren();
};

/**
 * This class provides a priority composite node for a behavior tree.
 *
 * A priority node is a composite node which is designed to run the nodes below
 * it with the highest priority values. A child node that is running may be
 * interrupted by another child node that has a higher priority value during the
 * update function.
 *
 * A priority node's state is directly based upon the child node currently running
 * or the child node that has finished running. Only one child node will finish
 * running as part of the PriorityNode.
 */
class PriorityNode : public CompositeNode {
	/**
	 * Returns a newly allocated PriorityNode with the given name.
	 *
	 * @param name  The name of the composite node.
	 *
	 * @return a newly allocated PriorityNode with the given name.
	 */
	static std::shared_ptr<PriorityNode> alloc(const std::string& name) {
		std::shared_ptr<PriorityNode> result = std::make_shared<PriorityNode>();
		return (result->init(name) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated PriorityNode with the given name and children.
	 *
	 * @param name  The name of the composite node.
	 * @param children The children of the composite node.
	 *
	 * @return a newly allocated PriorityNode with the given name and children.
	 */
	static std::shared_ptr<PriorityNode> alloc(const std::string& name,
											   const std::vector<std::shared_ptr<BehaviorNode>>& children) {
		std::shared_ptr<PriorityNode> result = std::make_shared<PriorityNode>();
		return (result->init(name, children) ? result : nullptr);
	}
	
	/**
	 * Returns the BehaviorNode::State of the priority node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * composite node (and all nodes below this node in the tree).
	 * The state for this node is derived from the state of the running
	 * or most recently run node.
	 *
	 * The priority value of the node is updated within this function, based
	 * on the priority values of the nodes below the given node.
	 *
	 * @return the BehaviorNode::State of the priority node.
	 */
	BehaviorNode::State update();
};

/**
 * This class provides a sequence composite node for a behavior tree.
 *
 * A sequence node is a composite node which is designed to run the nodes below
 * it in order, on the basis of success for previous nodes.
 *
 * The first child is run, and if it is successful, then the next child is run.
 * If any child fails, then the SequenceNode fails, while the SequenceNode succeeds
 * if all children running in sequence succeed. The node is running in the meantime.
 */
class SequenceNode : public CompositeNode {
	/**
	 * Returns a newly allocated SequenceNode with the given name.
	 *
	 * @param name  The name of the composite node.
	 *
	 * @return a newly allocated SequenceNode with the given name.
	 */
	static std::shared_ptr<SequenceNode> alloc(const std::string& name) {
		std::shared_ptr<SequenceNode> result = std::make_shared<SequenceNode>();
		return (result->init(name) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated SequenceNode with the given name and children.
	 *
	 * @param name  The name of the composite node.
	 * @param children The children of the composite node.
	 *
	 * @return a newly allocated SequenceNode with the given name and children.
	 */
	static std::shared_ptr<SequenceNode> alloc(const std::string& name,
											   const std::vector<std::shared_ptr<BehaviorNode>>& children) {
		std::shared_ptr<SequenceNode> result = std::make_shared<SequenceNode>();
		return (result->init(name, children) ? result : nullptr);
	}
	
	/**
	 * Returns the BehaviorNode::State of the sequence node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * composite node (and all nodes below this node in the tree).
	 * The state for this node is derived from the state of the running
	 * or most recently run node.
	 *
	 * The priority value of the node is updated within this function, based
	 * on the priority values of the nodes below the given node.
	 *
	 * @return the BehaviorNode::State of the sequence node.
	 */
	BehaviorNode::State update();
};

/**
 * This class provides a selector composite node for a behavior tree.
 *
 * A selector node is a composite node which is designed to run the nodes below
 * it in order, on the basis of failure for previous nodes. SelectorNode "selects"
 * one of the nodes below it as the option taken based on failure of the options
 * given as child nodes before it.
 *
 * The first node is run and if it is successful, then the SelectorNode's state
 * is set to success. Otherwise, the next node is run. If all child nodes fail,
 * then the SelectorNode has failed. It is running in the meantime.
 */
class SelectorNode : public CompositeNode {
	/**
	 * Returns a newly allocated SelectorNode with the given name.
	 *
	 * @param name  The name of the composite node.
	 *
	 * @return a newly allocated SelectorNode with the given name.
	 */
	static std::shared_ptr<SelectorNode> alloc(const std::string& name) {
		std::shared_ptr<SelectorNode> result = std::make_shared<SelectorNode>();
		return (result->init(name) ? result : nullptr);
	}
	
	/**
	 * Returns a newly allocated SelectorNode with the given name and children.
	 *
	 * @param name  The name of the composite node.
	 * @param children The children of the composite node.
	 *
	 * @return a newly allocated SelectorNode with the given name and children.
	 */
	static std::shared_ptr<SelectorNode> alloc(const std::string& name,
											   const std::vector<std::shared_ptr<BehaviorNode>>& children) {
		std::shared_ptr<SelectorNode> result = std::make_shared<SelectorNode>();
		return (result->init(name, children) ? result : nullptr);
	}
	
	/**
	 * Returns the BehaviorNode::State of the selector node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * composite node (and all nodes below this node in the tree).
	 * The state for this node is derived from the state of the running
	 * or most recently run node.
	 *
	 * The priority value of the node is updated within this function, based
	 * on the priority values of the nodes below the given node.
	 *
	 * @return the BehaviorNode::State of the selector node.
	 */
	BehaviorNode::State update();
};
	
	
}
#endif /* __CU_COMPOSITE_NODE_H__ */


