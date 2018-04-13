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
 * one child, and perform some function regarding the state of the child node or
 * the number of times the child node is run. There are rules specific to each
 * type of decorator node defining how many times the child is run and the
 * state of the decorator given the state of the child node.
 *
 * The three concrete subclasses for a DecoratorNode are: InverterNode,
 * RepeaterNode, and the TimedDelayNode.While similar in structure,
 * each class has key differences defining how it runs in relation to its child
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
	DecoratorNode();
	
	/**
	 * Deletes this node, disposing all resources.
	 */
	~DecoratorNode() { dispose(); }
	
	/**
	 * Disposes all of the resources used by this node.
	 *
	 * A disposed DecoratorNode can be safely reinitialized.
	 *
	 * It is unsafe to call this on a DecoratorNode that is still currently
	 * inside of a running behavior tree.
	 */
	void dispose() override;
	
	/**
	 * Initializes a decorator node with the given name.
	 *
	 * @param name  The name of the decorator node.
	 *
	 * @return true if initialization was successful.
	 */
	bool init(const std::string& name) override;
	
	/**
	 * Initializes a decorator node with the given name and child.
	 *
	 * @param name  The name of the decorator node.
	 * @param child The child of the decorator node.
	 *
	 * @return true if initialization was successful.
	 */
	bool initWithChild(const std::string& name, const std::shared_ptr<BehaviorNode>& child) {
		setChild(child);
		return init(name);
	}

	/**
	 * Initializes a decorator node with the given name, child, and priority
	 * function.
	 * 
	 * @param name The name of the decorator node.
	 * @param child The child of the decorator node.
	 * @param priority The priority function of the decorator node.
	 * 
	 * @return true if initalization was successful.	 
	 */
	bool initWithData(const std::string& name, const std::shared_ptr<BehaviorNode>& child,
					  const std::function<float()>& priority) {
		setChild(child);
		setPriorityFunction(priority);
		return init(name);
	}
	
#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Returns the node's child.
	 *
	 * @return the node's child.
	 */
	std::shared_ptr<BehaviorNode> getChild() { return _child; }
	
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
	 * While the child is a subclass of type BehaviorNode, you may want to access
	 * them by their specific subclass.  If the child is not an instance of type
	 * T (or a subclass), this method returns nullptr.
	 *
	 * @return the child at the given position, typecast to a shared T pointer.
	 */
	template <typename T>
	inline std::shared_ptr<T> getChild() const {
		return std::dynamic_pointer_cast<T>(getChild());
	}
	
	/**
	 * Sets the child of this node if this node is not currently locked.
	 *
	 * @param child The child node.
	 * 
	 * @return true if this node's child was successfully set.
	 *
	 * @warning this function will only run when node is not _locked.
	 */
	bool setChild(const std::shared_ptr<BehaviorNode>& child);
	
	/**
	 * Sets the child of this node with the given name if this node is not
	 * currently locked.
	 *
	 * @param child The child node.
	 * @param name  A string to identify the node.
	 * 
	 * @return true if this nodes' child was successfully set.
	 *
	 * @warning this function will only run when node is not _locked.
	 */
	bool setChildWithName(const std::shared_ptr<BehaviorNode>& child, const std::string &name) {
		child->setName(name);
		return setChild(child);
	}
	
	/**
	 * Removes the child of this DecoratorNode if this node is not currently
	 * locked.
	 * 
	 * @return true if this nodes' child was sucessfully removed.
	 *
	 * @warning this function will only run when node is not _locked.
	 */
	bool removeChild();
	
	/**
	 * Returns the BehaviorNode::State of the decorator node.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * decorator node (and all nodes below this node in the tree).
	 * The state for this node is derived from its child.
	 *
	 * The priority value of the node is updated within this function, based
	 * on the priority values of the nodes below the given node.
	 *
	 * @return the BehaviorNode::State of the decorator node.
	 */
	virtual BehaviorNode::State update() override = 0;
};
	
	
}
#endif /* __CU_DECORATOR_NODE_H__ */
