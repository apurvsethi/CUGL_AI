//
//  CUCompositeNode.cpp
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
//  Version: 5/22/2018
//

#include <algorithm>
#include <vector>
#include <cugl/ai/behaviorTree/CUCompositeNode.h>
#include <cugl/util/CUDebug.h>

using namespace cugl;

#pragma mark -
#pragma mark Constructors
/**
 * Initializes a composite node with the given name, type, children, and
 * priority function.
 *
 * @param name		The name of the composite node.
 * @param priority	The priority function of the composite node.
 * @param children 	The children of the composite node.
 * @param preempt	Whether child nodes can be preempted.
 *
 * @return true if initialization was successful.
 */
bool CompositeNode::init(const std::string& name,
						 const std::function<float()> priority,
						 const std::vector<std::shared_ptr<BehaviorNode>>& children,
						 bool preempt) {
	_preempt = preempt;
	return BehaviorNode::init(name, priority, children);
}

/**
 * Disposes all of the resources used by this node, and all descendants
 * in the tree.
 *
 * A disposed CompositeNode can be safely reinitialized.
 *
 * It is unsafe to call this on a CompositeNode that is still currently
 * inside of a running behavior tree.
 */
void CompositeNode::dispose() {
	BehaviorNode::dispose();
	_preempt = false;
}

#pragma mark -
#pragma mark Behavior Tree
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
const BehaviorNode* CompositeNode::getChild(unsigned int pos) const {
	CUAssertLog(pos < _children.size(), "Position index out of bounds");
	return _children[pos].get();
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
const BehaviorNode* CompositeNode::getChildByName(const std::string& name) const {
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		if ((*it)->getName() == name) {
			return (*it).get();
		}
	}
	return nullptr;
}

/**
 * Returns a (weak) reference to the child with the given priority index.
 *
 * A child with a specific priority index i is the child with the ith
 * highest priority. Ties are by the position of the child in its
 * parent's list.
 *
 * As a weak reference, this composite node does not pass ownership of its
 * child.
 *
 * @param index	The child's priority index.
 *
 * @return a (weak) reference the child with the given priority index.
 */
const BehaviorNode* CompositeNode::getChildByPriorityIndex(unsigned int index) const {
	CUAssertLog(index < _children.size(), "Priority index out of bounds");
	std::vector<std::shared_ptr<BehaviorNode>> ordered_children = _children;
	std::sort(ordered_children.begin(), ordered_children.end(), BehaviorNode::compareNodeSibs);
	return ordered_children[index].get();
}

/**
 * Returns a (weak) reference to the currently active child.
 *
 * As a weak reference, this composite node does not pass ownership of its
 * child.
 *
 * @return a (weak) reference to the currently active child.
 */
const BehaviorNode* CompositeNode::getActiveChild() const {
	CUAssertLog(_activeChildPos != -1, "No children are currently running.");
	return _children[_activeChildPos].get();
}

/**
 * Returns the list of (weak) references to the node's children.
 *
 * As weak references, this composite node does not pass ownership of its
 * children.
 *
 * @return the list of (weak) references the node's children.
 */
std::vector<const BehaviorNode*> CompositeNode::getChildren() const {
	std::vector<const BehaviorNode*> children;
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		children.push_back((*it).get());
	}
	return children;
}

/**
 * Updates the priority value for this node and all children beneath it,
 * running the piority function provided or default priority function
 * if available for the class.
 */
void CompositeNode::updatePriority() {
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		(*it)->updatePriority();
	}
	if (_priorityFunc) {
		setPriority(_priorityFunc());
	}
	else if (!_preempt && _activeChildPos != -1) {
		setPriority(_children[_activeChildPos]->getPriority());
	}
	else {
		setPriority(getChosenChild()->getPriority());
	}
}

/**
 * Updates this node and its children.
 *
 * Runs an update function, meant to be used on each tick, for the
 * behavior node (and nodes chosen to run below it in the tree).
 *
 * Update priority may be run as part of this function, based on whether a
 * composite node uses preemption.
 *
 * @param dt	The elapsed time since the last frame.
 *
 * @return @return the state of this node after updating.
 */
BehaviorNode::State CompositeNode::update(float dt) {
	if (_activeChildPos != -1 && _preempt) {
		updatePriority();
	}

	if (getState() != BehaviorNode::State::RUNNING) {
		for (auto it = _children.begin(); it != _children.end(); ++it) {
			(*it)->update(dt);
		}
		return getState();
	}

	std::shared_ptr<BehaviorNode> activeChild;
	if (_activeChildPos == -1 || _preempt) {
		activeChild = getChosenChild();
		if (_activeChildPos != -1 && _children[_activeChildPos] != activeChild) {
			_children[_activeChildPos]->preempt();
		}
		_activeChildPos = activeChild->getChildOffset();
	}
	activeChild = _children[_activeChildPos];

	activeChild->setState(BehaviorNode::State::RUNNING);
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		(*it)->update(dt);
	}
	setState(activeChild->getState());
	return getState();
}
