//
//  CUPriorityNode.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a priority composite behavior node.
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
#include <sstream>
#include <cugl/ai/behaviorTree/CUPriorityNode.h>

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
std::string PriorityNode::toString(bool verbose) const {
	std::stringstream ss;
	ss << (verbose ? "cugl::PriorityNode(name:" : "(name:") << _name;
	ss << ", priority:" << _priority;
	ss << ", children[";
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		ss << (*it)->getName() << ",";
	}
	ss << "])";
	return ss.str();
}

#pragma mark -
#pragma mark Internal Helper

/**
 * Returns the child choosen by this priority node.
 *
 * A priority node will choose the child with the maximum priority, with ties broken
 * by the child with the earliest position.
 *
 * @return the child choosen by this priority node.
 */
const std::shared_ptr<BehaviorNode>& PriorityNode::getChosenChild() const {
	return *std::min_element(_children.begin(), _children.end(),
							 BehaviorNode::compareNodeSibs);
}
