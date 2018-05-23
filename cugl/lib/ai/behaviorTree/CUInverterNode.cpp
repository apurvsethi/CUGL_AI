//
//  CUInverterNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for an inverter decorator behavior node.
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
#include <cugl/ai/behaviorTree/CUInverterNode.h>

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
std::string InverterNode::toString(bool verbose) const {
	std::stringstream ss;
	ss << (verbose ? "cugl::InverterNode(name:" : "(name:") << _name;
	ss << ", priority:" << _priority;
	ss << ", child:" << (_children[0] ? _children[0]->getName() : "None");
	ss << ")";
	return ss.str();
}
