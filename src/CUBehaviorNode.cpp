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
		if ((*it)->_name == name) {
			return *it;
		}
	}
	return nullptr;
}

#pragma mark -
#pragma mark Constructors
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
	_state = BehaviorNode::State::UNINITIALIZED;
	_priority = 0;
	_priorityFunc = nullptr;
	removeFromParent();
	_parent = nullptr;
	_childOffset = -2;
}

#pragma mark -
#pragma mark Internal Helpers

/**
* Returns true if sibling a has a larger priority than sibling 2.
*
* This method is used by std::sort to sort the children. Ties are
* broken from the offset of the children.
*
* @param a The first child
* @param b The second child
*
* @return true if sibling a is has a larger priority than sibling 2.
*/
bool BehaviorNode::compareNodeSibs(const std::shared_ptr<BehaviorNode>& a, const std::shared_ptr<BehaviorNode>& b) {
	return a->_priority > b->_priority 
	|| (a->_priority == b->_priority && a->_childOffset > b->_childOffset);
}

/**
* Removes the child at the given position from this node.
*
* @param pos   The position of the child node which will be removed.
*/
void removeChild(unsigned int pos) {
	CULogError("Cannot remove child from this node.");
}