//
//  CUInverterNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for an inverter decorator behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
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
	ss << ",priority:" << _priority;
	ss << ",child:" << (_child ? _child->getName() : "None");
	ss << ")";
	return ss.str();
}
