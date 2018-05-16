//
//  CUBehaviorNode.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a behavior node as part of a
//  behavior tree.
//
//  You should never instantiate an object of this class.  Instead, you should
//  use one of the concrete subclasses of BehaviorNode. Because this is an
//  abstract class, it has no allocators.  It only has an initializer.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#include <cugl/ai/behaviorTree/CUBehaviorNode.h>
#include <cugl/util/CUDebug.h>
#include <sstream>

using namespace cugl;

#pragma mark Behavior Node Defintion
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
std::shared_ptr<BehaviorNodeDef> BehaviorNodeDef::getNodeByName(const std::string& name) {
	if (name == _name) {
		return shared_from_this();
	}
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		std::shared_ptr<BehaviorNodeDef> result = getNodeByName(name);
		if (result) {
			return result;
		}
	}
	return nullptr;
}

#pragma mark -
#pragma mark Constructors
/**
 * Initializes a behavior node with the given name, children, and priority
 * function.
 *
 * @param name		The name of the behavior node
 * @param priority	The priority function of the behavior node
 * @param children 	The children of the behavior node
 *
 * @return true if initialization was successful.
 */
bool BehaviorNode::init(const std::string& name,
						const std::function<float()> priority,
						const std::vector<std::shared_ptr<BehaviorNode>>& children) {
	_name = name;
	_priorityFunc = priority;
	_childOffset = -1;
	for (int ii = 0; ii < _children.size(); ii++) {
		_children[ii]->setParent(this);
		_children[ii]->_childOffset = ii;
	}
	return true;
}

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
void BehaviorNode::dispose() {
	_name = "";
	_state = BehaviorNode::State::UNINITIALIZED;
	_priority = 0;
	_priorityFunc = nullptr;
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		(*it)->_parent = nullptr;
		(*it)->_childOffset = -1;
	}
	removeFromParent();
	_parent = nullptr;
	_childOffset = -2;
}

#pragma mark -
#pragma mark Behavior Trees
/**
 * Sets the state of this node.
 *
 * This state is used to identify the state of this node. If this node
 * has no parent, then this is the state of the behavior tree.
 *
 * @param state The state of this node.
 */
void BehaviorNode::setState(BehaviorNode::State state) {
	CUAssertLog(state != BehaviorNode::State::RUNNING || getPriority() != 0.0f,
				"Running node cannot have 0 priority.");
	_state = state;
}

/**
 * Reset this node and all nodes below it to an uninitialized state. Also
 * resets any class values to those set at the start of the tree.
 */
void BehaviorNode::reset() {
	setState(BehaviorNode::State::UNINITIALIZED);
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		(*it)->reset();
	}
}

/**
 * Pause this running node and all running nodes below it in the tree,
 * allowing them to be resumed later.
 *
 * This method has no effect on values stored within nodes, and values will
 * not be updated while nodes are paused.
 */
void BehaviorNode::pause() {
	CUAssertLog(getState() == BehaviorNode::State::RUNNING,
				"Cannot pause a node that is not currently running.");
	setState(BehaviorNode::State::PAUSED);
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		if ((*it)->getState() == BehaviorNode::State::RUNNING) {
			(*it)->pause();
		}
	}
}

/**
 * Resumes a paused node and all paused nodes below it in the tree, allowing
 * them to run again.
 *
 * Values such as priority or delay for a timer node will not have
 * been updated while the node was paused.
 */
void BehaviorNode::resume() {
	CUAssertLog(getState() == BehaviorNode::State::PAUSED,
				"Cannot resume a node that is not currently paused.");
	setState(BehaviorNode::State::RUNNING);
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		if ((*it)->getState() == BehaviorNode::State::PAUSED) {
			(*it)->resume();
		}
	}
}

/**
 * Begin running this node, moving from an uninitialized state to a running
 * state as the correct action to perform as all priority values are updated
 * and the correct node to run is found through choosing a leaf node.
 */
void BehaviorNode::start() {
	updatePriority();
	setState(BehaviorNode::State::RUNNING);
	update(0.0f);
}

#pragma mark -
#pragma mark Internal Helpers
/**
 * Sets the priority of this node.
 *
 * @param priority The priority of this node.
 */
void BehaviorNode::setPriority(float priority) {
	CUAssertLog(priority >= 0.0f && priority <= 1.0f,
				"Priority must be between 0 and 1");
	_priority = priority;
}

/**
* Removes the child at the given position from this node.
*
* @param pos   The position of the child node which will be removed.
*/
void BehaviorNode::removeChild(unsigned int pos) {
	CUAssertLog(pos < _children.size(), "Index out of bounds");
	std::shared_ptr<BehaviorNode> child = _children[pos];
	child->setParent(nullptr);
	child->_childOffset = -1;
	for (int ii = pos; ii < _children.size() - 1; ii++) {
		_children[ii] = _children[ii + 1];
		_children[ii]->_childOffset = ii;
	}
	_children.resize(_children.size() - 1);
}

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
bool BehaviorNode::compareNodeSibs(const std::shared_ptr<BehaviorNode>& a, const std::shared_ptr<BehaviorNode>& b) {
	return a->_priority > b->_priority
		|| (a->_priority == b->_priority && a->_childOffset < b->_childOffset);
}
