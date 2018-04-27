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

#include <cugl/ai/behaviorTree/CUBehaviorTree.h>
#include <sstream>

using namespace cugl;

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
    BehaviorNode namedChild;
    for (auto it = _children.begin(); it != _children.end(); ++it) {
        namedChild = (*it)->getNodeByName();
        if (namedChild) {
            return namedChild;
        }
    }
    return nullptr;
}

#pragma mark -
#pragma mark Constructors
/**
 * Initializes a node using the given template def.
 *
 * @param behaviorNodeDef	The def specifying arguments for this node.
 *
 * @return true if initialization was successful.
 */
bool init(const std::shared_ptr<BehaviorNodeDef>& behaviorNodeDef) {
    _name = behaviorNodeDef->_name;
    _parent = behaviorNodeDef->_parent;
    _state = BehaviorNode::State::UNINITIALIZED;
    _priority = 0;
    _priorityFunc = behaviorNodeDef->_priorityFunc;
    return true;
}

/**
 * Disposes all of the resources used by this node.
 *
 * A disposed BehaviorNode can be safely reinitialized.
 *
 * It is unsafe to call this on a BehaviorNode that is still currently
 * inside of a running behavior tree.
 */
void BehaviorNode::dispose() {
    _name = "";
    //TODO: Remove child from parent.
    _parent = nullptr;
    _state = BehaviorNode::State::UNINITIALIZED;
    _priority = 0;
    _priorityFunc = nullptr;
}

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
std::string toString(bool verbose = false) const {
    // Override?
}