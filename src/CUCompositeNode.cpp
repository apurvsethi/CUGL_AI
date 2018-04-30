//
//  CUCompositeNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a composite behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#include <cugl/ai/behaviorTree/CUCompositeNode.h>
#include <cugl/util/CUDebug.h>
#include <algorithm>
#include <vector>

using namespace cugl;

#pragma mark -
#pragma mark Constructors
/**
* Initializes a composite node with the given name and children.
*
* The priority of this node is dependant of the children's priorities.
*
* @param name		The name of the composite node.
* @param children 	The children of the composite node.
* @param preempt	Whether child nodes can be preempted.
*
* @return true if initialization was successful.
*/
bool CompositeNode::init(const std::string& name,
						 const std::vector<std::shared_ptr<BehaviorNode>>& children,
	                     bool preempt = false) {
	_name = name;
	_children = children;
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		(*it)->setParent(this);
		(*it)->setChildOffset(it - _children.begin());
	}
	_preempt = preempt;
	return true;
}

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
bool CompositeNode::init(const std::string& name,
						 const std::function<float()> priority,
						 const std::vector<std::shared_ptr<BehaviorNode>>& children,
						 bool preempt = false) {
	_priorityFunc = priority;
	return init(name, children, preempt);
}

/**
 * Disposes all of the resources used by this node.
 *
 * A disposed CompositeNode can be safely reinitialized.
 *
 * It is unsafe to call this on a CompositeNode that is still currently
 * inside of a running behavior tree.
 */
void CompositeNode::dispose() {
    BehaviorNode::dispose();
    _preempt = false;
    for (auto it = _children.begin(); it != _children.end(); ++it) {
		(*it)->setParent(nullptr);
		(*it)->setChildOffset(-1);
		//TODO: Dispose children instead?
	}
    _children.clear();
	removeFromParent();
	_parent = nullptr;
}

#pragma mark -
#pragma mark Behavior Tree

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
const std::shared_ptr<BehaviorNode>& CompositeNode::getChild(unsigned int pos) const {
	CUAssertLog(pos < _children.size(), "Position index out of bounds");
    return _children[pos];
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
const std::shared_ptr<BehaviorNode>& CompositeNode::getChildByName(const std::string& name) const {
    for (auto it = _children.begin(); it != _children.end(); ++it) {
        if ((*it)->getName() == name) {
            return *it;
        }
    }
    return nullptr;
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
const std::shared_ptr<BehaviorNode>& CompositeNode::getChildByPriorityIndex(unsigned int index) const {
    CUAssertLog(index < _children.size(), "Priority index out of bounds");
    std::vector<std::shared_ptr<BehaviorNode>> ordered_children = _children;
    std::sort(ordered_children.begin(), ordered_children.end(), BehaviorNode::compareNodeSibs);
    return ordered_children[index];
}

#pragma mark -
#pragma mark Internal Helpers
/**
 * Removes the child at the given position from this node.
 *
 * @param pos   The position of the child node which will be removed.
 */
void CompositeNode::removeChild(unsigned int pos) {
	CUAssertLog(pos < _children.size(), "Index out of bounds");
	std::shared_ptr<BehaviorNode> child = _children[pos];
	child->setParent(nullptr);
	child->setChildOffset(-1);
	for (int ii = pos; ii < _children.size() - 1; ii++) {
		_children[ii] = _children[ii + 1];
		_children[ii]->setChildOffset(ii);
	}
	_children.resize(_children.size() - 1);
}