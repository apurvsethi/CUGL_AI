//
//  CURandomNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a random composite behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#include <sstream>
#include <cugl/ai/behaviorTree/CURandomNode.h>

using namespace cugl;

#pragma mark -
#pragma mark Constructors
/**
 * Disposes all of the resources used by this node, and all descendants
 * in the tree.
 *
 * A disposed RandomNode can be safely reinitialized.
 *
 * It is unsafe to call this on a RandomNode that is still currently
 * inside of a running behavior tree.
 */
void RandomNode::dispose() {
	CompositeNode::dispose();
	_uniformRandom = true;
}

/**
 * Initializes a random node with the given name, children, priority
 * function, and random type.
 *
 * @param name			The name of the random node.
 * @param priority		The priority function of the random node.
 * @param children 		The children of the random node.
 * @param preempt		Whether child nodes can be preempted.
 * @param uniformRandom	Whether children are chosen uniformly at random.
 *
 * @return true if initialization was successful.
 */
bool RandomNode::init(const std::string& name,
					  const std::function<float()> priority,
					  const std::vector<std::shared_ptr<BehaviorNode>>& children,
					  bool preempt, bool uniformRandom) {
	_uniformRandom = uniformRandom;
	return CompositeNode::init(name, priority, children, preempt);
}

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
std::string RandomNode::toString(bool verbose) const {
	std::stringstream ss;
	ss << (verbose ? "cugl::RandomNode(name:" : "(name:");
	ss << "priority" << _priority;
	ss << "random type:" << (_uniformRandom ? "uniform" : "weighted");
	ss << "children:[";
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		ss << (*it)->getName();
	}
	ss << "])";
	return ss.str();
}

#pragma mark -
#pragma mark Behavior Tree
/**
 * Updates the priority value for this node and all children beneath it,
 * running the piority function provided or default priority function
 * if available for the class.
 */
void RandomNode::updatePriority() {
	float priority_sum = 0.0f;
	for (auto it = _children.begin(); it != _children.end(); ++it) {
		(*it)->updatePriority();
		priority_sum += (*it)->getPriority();
	}
	if (_priorityFunc) {
		setPriority(_priorityFunc());
	}
	else if (_activeChildPos != -1) {
		setPriority(_children[_activeChildPos]->getPriority());
	}
	else {
		setPriority(priority_sum / _children.size());
	}
}

#pragma mark -
#pragma mark Internal Helpers
/**
 * Returns the child with the smallest position which has a non-zero priority.
 *
 * @return the first child with a non-zero priority.
 */
const std::shared_ptr<BehaviorNode>& RandomNode::getChosenChild() const {
	if (!_uniformRandom) {
		float priority_sum = 0.0f;
		for (auto it = _children.begin(); it != _children.end(); ++it) {
			priority_sum += (*it)->getPriority();
		}
		if (priority_sum > 0.0f) {
			float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX / priority_sum);
			for (auto it = _children.begin(); it != _children.end(); ++it) {
				if (r < (*it)->getPriority()) {
					return *it;
				}
				r -= (*it)->getPriority();
			}
		}
	}

	return _children[rand() % _children.size()];
}
