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
#include <algorithm>

using namespace cugl;

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
        *it->_parent = nullptr;
    }
    _children.clear();
    //TODO: Remove from parent.
}

/**
 * Initializes a composite node using the given template def.
 *
 * @param behaviorNodeDef	The def specifying arguments for this node.
 *
 * @return true if initialization was successful.
 */
void CompositeNode::init(const std::shared_ptr<BehaviorNodeDef>& behaviorNodeDef) {
    if (behaviorNodeDef->_childCount == 0) {
        return false;
    }
    _preempt = behaviorNodeDef->_preempt;
    for (auto it = BehaviorNodeDef->_children.begin(); it != behaviorNodeDef->children._end(); ++it) {
        //TODO: Handle this case.
    }
}

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
void CompositeNode::getChild(unsigned int pos) {
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
        if (*it->getName() == name) {
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
const std::shared_ptr<BehaviorNode>& getChildByPriorityIndex(unsigned int index) const {
    // TODO: Decide whether to maintain priority ordering or to do by default.
    CUAssertLog(index < _children.size(), "Priority index out of bounds");
    std::vector<std::shared_ptr<BehaviorNode>>& orderedChildren = _children;
    std::sort(orderedChildren.begin(), orderedChildren.end(),
        [](const std::shared_ptr<BehaviorNode>& child1, const std::shared_ptr<BehaviorNode>& child2)
        {
            return child1->getPriority() > child2->getPriority();
        }
    );
    return orderedChildren[index];
}