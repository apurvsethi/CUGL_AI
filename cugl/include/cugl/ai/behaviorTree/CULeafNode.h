//
//  CULeafNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a leaf behavior node.
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
//  Version: 5/21/2018
//

#ifndef __CU_LEAF_NODE_H__
#define __CU_LEAF_NODE_H__

#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUBehaviorNode.h>
#include <cugl/ai/behaviorTree/CUBehaviorAction.h>

namespace cugl {

/**
 * This class provides a leaf behavior node for a behavior tree.
 *
 * A leaf node within a behavior tree refers to the base nodes that perform
 * actions based on conditionals. Each leaf node has a user defined priority
 * function which it will call each update tick to set its priority.
 * Additionally, each leaf node also has a provided action which begins running
 * when the leaf node is chosen for execution.
 */
class LeafNode : public BehaviorNode {
#pragma mark Values
protected:
	/**
	 * The action used when this node is run.
	 */
	std::shared_ptr<BehaviorAction> _action;

#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized leaf node.
	 *
	 * You must initialize this LeafNode before use.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	LeafNode() : BehaviorNode(), _action(nullptr) {};

	/**
	 * Deletes this node, disposing all resources.
	 */
	~LeafNode() { dispose(); }

	/**
	 * Disposes all of the resources used by this node.
	 *
	 * A disposed LeafNode can be safely reinitialized.
	 *
	 * It is unsafe to call this on a LeafNode that is still currently
	 * inside of a running behavior tree.
	 */
	void dispose() override;

	/**
	 * Initializes a leaf node with the given name, priority function, and
	 * action.
	 *
	 * @param name 		The name of the leaf node.
	 * @param priority 	The priority function of the leaf node.
	 * @param action 	The action of the leaf node.
	 *
	 * @return true if initialization was successful.
	 */
	bool init(const std::string& name, const std::function<float()>& priority,
			  const std::shared_ptr<BehaviorAction>& action);

#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated LeafNode with the given name, priority
	 * function, and action.
	 *
	 * @param name 		The name of the leaf node.
	 * @param priority 	The priority function of the leaf node.
	 * @param action 	The action of the leaf node.
	 *
	 * @return a newly allocated LeafNode with the given name, priority
	 * function, and action.
	 */
	static std::shared_ptr<LeafNode> alloc(const std::string& name,
										   const std::function<float()>& priority,
										   const std::shared_ptr<BehaviorAction>& action) {
		std::shared_ptr<LeafNode> result = std::make_shared<LeafNode>();
		return (result->init(name, priority, action) ? result : nullptr);
	}

#pragma mark -
#pragma mark Identifier
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
	std::string toString(bool verbose = false) const override;

#pragma mark -
#pragma mark Behavior Tree
	/**
	 * Return the action used by this leaf node.
	 *
	 * @return the action used by this leaf node.
	 */
	const BehaviorAction* getAction() const { return _action.get(); }

	/**
	 * Updates the priority value for this node and all children beneath it,
	 * running the piority function provided or default priority function
	 * if available for the class.
	 */
	void updatePriority() override {
		_priority = _priorityFunc();
	}

	/**
	 * Reset this node and all nodes below it to an uninitialized state. Also
	 * resets any class values to those set at the start of the tree.
	 */
	void reset() override;
	/**
	 * Pause this running node and all running nodes below it in the tree,
	 * allowing them to be resumed later.
	 *
	 * This method has no effect on values stored within nodes, and values will
	 * not be updated while nodes are paused.
	 */
	void pause() override;

	/**
	 * Resumes a paused node and all paused nodes below it in the tree, allowing
	 * them to run again.
	 *
	 * Values such as priority or delay for a timer node will not have
	 * been updated while the node was paused.
	 */
	void resume() override;

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
	void preempt() override {
		_action->terminate();
		setState(BehaviorNode::State::INACTIVE);
	}
};


}
#endif /* __CU_LEAF_NODE_H__ */
