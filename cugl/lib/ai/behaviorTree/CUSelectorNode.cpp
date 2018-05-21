//
//  CUSelectorNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a selector composite behavior node.
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

#include <sstream>
#include <cugl/ai/behaviorTree/CUSelectorNode.h>

using namespace cugl;

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
std::string SelectorNode::toString(bool verbose) const {
	std::stringstream ss;
	ss << (verbose ? "cugl::SelectorNode(name:" : "(name:");
	ss << ", priority:" << _priority;
	ss << ", children:[";
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		ss << (*it)->getName();
	}
	ss << "])";
	return ss.str();
}

#pragma mark -
#pragma mark Internal Helpers
/**
 * Returns the child choosen by this selector node.
 *
 * A selector node will choose the first child with a nonzero priority. If all
 * children have a priority of zero, the selector node will choose the first
 * child.
 *
 * @return the child choosen by this selector node.
 */
const std::shared_ptr<BehaviorNode>& SelectorNode::getChosenChild() const {
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		if ((*it)->getPriority() > 0) {
			return *it;
		}
	}
	return _children[0];
}
