//
//  CULeafNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a leaf behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#include <cugl/ai/behaviorTree/CULeafNode.h>
#include <cugl/util/CUDebug.h>
#include <sstream>

using namespace cugl;

#pragma mark -
#pragma mark Constructors
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
bool LeafNode::init(const std::string& name, const std::function<float()>& priority,
					const std::shared_ptr<BehaviorAction>& action) {
	_action = action;
	return BehaviorNode::init(name, priority);;
}


/**
 * Disposes all of the resources used by this node.
 *
 * A disposed LeafNode can be safely reinitialized.
 *
 * It is unsafe to call this on a LeafNode that is still currently
 * inside of a running behavior tree.
 */
void LeafNode::dispose() {
	BehaviorNode::dispose();
	_action = nullptr;
}

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
std::string LeafNode::toString(bool verbose) const {
	std::stringstream ss;
	ss << (verbose ? "cugl::LeafNode(name:" : "(name:") << _name;
	ss << ", priority:" << _priority;
	ss << ", action:" << (_action ? _action->getName() : "None");
	ss << ")";
	return ss.str();
}

#pragma mark -
#pragma mark Behavior Tree
/**
 * Pause this running node and all running nodes below it in the tree,
 * allowing them to be resumed later.
 *
 * This method has no effect on values stored within nodes, and values will
 * not be updated while nodes are paused.
 */
void LeafNode::pause() {
	CUAssertLog(getState() == BehaviorNode::State::RUNNING,
		"Cannot pause a node that is not currently running.");
	_action->pause();
	setState(BehaviorNode::State::PAUSED);
}

/**
 * Resumes a paused node and all paused nodes below it in the tree, allowing
 * them to run again.
 *
 * Values such as priority or delay for a timer node will not have
 * been updated while the node was paused.
 */
void LeafNode::resume() {
	CUAssertLog(getState() == BehaviorNode::State::PAUSED,
		"Cannot resume a node that is not currently paused.");
	setState(BehaviorNode::State::RUNNING);
	_action->resume();
}

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
BehaviorNode::State LeafNode::update(float dt) {
	if (getState() != BehaviorNode::State::RUNNING) {
		return getState();
	}

	if (_action->getState() == BehaviorAction::State::UNINITIALIZED) {
		_action->start();
	}
	switch(_action->update(dt)) {
		case BehaviorAction::State::RUNNING:
			setState(BehaviorNode::State::RUNNING);
			break;
		case BehaviorAction::State::FINISHED:
			setState(BehaviorNode::State::FINISHED);
			break;
		default:
			break;
	}
	return getState();
}
