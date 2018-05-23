//
//  CUBehaviorManager.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a behavior tree manager. The behavior tree
//  manager controls the creation and execution of behavior trees.
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
//

#include <cstdlib>
#include <ctime>
#include <cugl/ai/behaviorTree/CUBehaviorManager.h>
#include <cugl/ai/behaviorTree/CUPriorityNode.h>
#include <cugl/ai/behaviorTree/CUSelectorNode.h>
#include <cugl/ai/behaviorTree/CURandomNode.h>
#include <cugl/ai/behaviorTree/CUInverterNode.h>
#include <cugl/ai/behaviorTree/CUTimerNode.h>
#include <cugl/ai/behaviorTree/CULeafNode.h>
#include <cugl/util/CUDebug.h>

using namespace cugl;

#pragma mark Constructors
/**
 * Disposes all of the resources used by this manager.
 *
 * It is unsafe to call this while behavior trees are running.
 */
void BehaviorManager::dispose() {
	_trees.clear();
}

/**
 * Initializes a behavior tree manager.
 *
 * @return true if initialization was successful.
 */
bool BehaviorManager::init() {
	std::srand(static_cast<unsigned int>(time(NULL)));
	return true;
}

#pragma mark -
#pragma mark Behavior Trees
/**
* Returns whether this manager contains a tree with the given name.
*
* @param name	An identifier to find the tree.
*
* @return whether this manager contains a tree with the given name.
*/
bool BehaviorManager::containsTree(const std::string& name) const {
	auto tree = _trees.find(name);
	return (tree != _trees.end());
}

/**
 * Returns a (weak) reference to the behavior tree with the given name.
 *
 * All trees must be stored with a unique names in the BehaviorManager,
 * and thus there cannot be multiple possible return values.
 *
 * By returning a weak reference, this manager does not pass ownership of
 * the tree.
 *
 * @param name	An identifier to find the tree.
 *
 * @return a (weak) reference to the tree with the given name.
 */
const BehaviorNode* BehaviorManager::getTree(const std::string& name) const {
	CUAssertLog(_trees.find(name) != _trees.end(),
				"Tree with given name does not exist in BehaviorManager.");
	return _trees.at(name).get();
}

/**
 * Starts running the tree with the given name.
 *
 * All trees must be stored with unique names in the BehaviorManager,
 * and thus there cannot be multiple possible trees to start.
 *
 * @param name	An identifier to find the tree.
 */
void BehaviorManager::startTree(const std::string& name) {
	CUAssertLog(_trees.find(name) != _trees.end(),
				"Tree with given name does not exist in BehaviorManager.");
	_trees.at(name)->start();
}

/**
 * Pauses the running tree with the given name.
 *
 * All trees must be stored with unique names in the BehaviorManager,
 * and thus there cannot be multiple possible trees to start.
 *
 * @param name	An identifier to find the tree.
 */
void BehaviorManager::pauseTree(const std::string& name) {
	CUAssertLog(_trees.find(name) != _trees.end(),
				"Tree with given name does not exist in BehaviorManager.");
	if (_trees.at(name)->getState() == BehaviorNode::State::RUNNING) {
		_trees.at(name)->pause();
	}
}

/**
 * Resumes running the paused tree with the given name.
 *
 * All trees must be stored with unique names in the BehaviorManager,
 * and thus there cannot be multiple possible trees to start.
 *
 * @param name	An identifier to find the tree.
 */
void BehaviorManager::resumeTree(const std::string& name) {
	CUAssertLog(_trees.find(name) != _trees.end(),
				"Tree with given name does not exist in BehaviorManager.");
	if (_trees.at(name)->getState() == BehaviorNode::State::PAUSED) {
		_trees.at(name)->resume();
	}
}

/**
 * Remove the tree with the given name, if the tree is not currently
 * running.
 *
 * All trees must be stored with unique names in the BehaviorManager,
 * and thus there cannot be multiple possible return values.
 *
 * @param name	An identifier to find the tree.
 */
void BehaviorManager::removeTree(const std::string& name) {
	CUAssertLog(_trees.find(name) != _trees.end(),
				"Tree with given name does not exist in BehaviorManager.");
	if (_trees.at(name)->getState() != BehaviorNode::State::RUNNING) {
		_trees.erase(name);
	}
}

/**
 * Restarts the tree with the given name, if the tree is finished.
 *
 * All trees must be stored with unique names in the BehaviorManager,
 * and thus there cannot be multiple possible return values.
 *
 * @param name	An identifier to find the tree.
 */
void BehaviorManager::restartTree(const std::string& name) {
	CUAssertLog(_trees.find(name) != _trees.end(),
				"Tree with given name does not exist in BehaviorManager.");
	if (_trees.at(name)->getState() == BehaviorNode::State::FINISHED) {
		_trees.at(name)->reset();
		_trees.at(name)->start();
	}
}

/**
 * Runs an update function, meant to be used on each tick, for each
 * behavior tree that is currently running within the manager.
 *
 * @param dt	The elapsed time since the last frame.
 */
void BehaviorManager::update(float dt) {
	for(auto it = _trees.begin(); it != _trees.end(); ++it) {
		if (it->second->getState() == BehaviorNode::State::RUNNING) {
			it->second->update(dt);
		}
	}
}

/**
 * Adds the behavior tree described by the provided definition to the
 * manager.
 *
 * Recursively creates a behavior tree from the template provided by
 * BehaviorNodeDefs, and adds it to the BehaviorManager. Returns false if
 * the BehaviorNodeDef provided does not allow creation of a valid
 * BehaviorNode, or if the name provided to the treeDef is equal to a name
 * provided for another tree in the manager, true otherwise.
 *
 * @param treeDef	The definition for the root of the behavior tree.
 *
 * @return whether the behaivor tree was successfully created and added.
 */
bool BehaviorManager::addTree(const std::shared_ptr<BehaviorNodeDef>& treeDef) {
	CUAssertLog(_trees.find(treeDef->_name) == _trees.end(),
				"Tree with given name already exists in BehaviorManager.");
	std::shared_ptr<BehaviorNode> tree = createTree(treeDef);
	if (tree == nullptr) {
		return false;
	}
	_trees[tree->getName()] = tree;
	return true;
}

/**
 * Creates the behavior tree created from the provided definition.
 *
 * Recursively creates a behavior tree from the template provided by the
 *{@link BehaviorNodeDef} of the root. This method will fail if the
 * behavior node definition does not define a valid behavior tree.
 *
 * @param treeDef	The definition for the root of the behavior tree.
 *
 * @return The behavior tree created from provided definition.
 */
std::shared_ptr<BehaviorNode> BehaviorManager::createTree(const std::shared_ptr<BehaviorNodeDef>& treeDef) {
	if (treeDef->_type == BehaviorNodeDef::Type::INVERTER_NODE) {
		CUAssertLog(treeDef->_children.size() == 1,
					"Incorrect number of children for decorator node");
		return InverterNode::alloc(treeDef->_name,
								   createTree(treeDef->_children[0]));
	}
	else if (treeDef->_type == BehaviorNodeDef::Type::TIMER_NODE) {
		CUAssertLog(treeDef->_children.size() == 1,
					"Incorrect number of children for decorator node");
		return TimerNode::alloc(treeDef->_name,
								createTree(treeDef->_children[0]),
								treeDef->_timeDelay, treeDef->_delay);
	}
	else if (treeDef->_type == BehaviorNodeDef::Type::PRIORITY_NODE) {
		CUAssertLog(treeDef->_children.size() > 0,
					"Incorrect number of children for composite node");
		return PriorityNode::alloc(treeDef->_name, treeDef->_priorityFunc,
								   createTrees(treeDef->_children),
								   treeDef->_preempt);
	}
	else if (treeDef->_type == BehaviorNodeDef::Type::SELECTOR_NODE) {
		CUAssertLog(treeDef->_children.size() > 0,
					"Incorrect number of children for composite node");
		return SelectorNode::alloc(treeDef->_name, treeDef->_priorityFunc,
								   createTrees(treeDef->_children),
								   treeDef->_preempt);
	}
	else if (treeDef->_type == BehaviorNodeDef::Type::RANDOM_NODE) {
		CUAssertLog(treeDef->_children.size() > 0,
					"Incorrect number of children for composite node");
		return RandomNode::alloc(treeDef->_name, treeDef->_priorityFunc,
								 createTrees(treeDef->_children),
								 treeDef->_preempt, treeDef->_uniformRandom);
	}
	else {
		CUAssertLog(treeDef->_children.size() == 0,
					"Incorrect number of children for leaf node");
		return LeafNode::alloc(treeDef->_name, treeDef->_priorityFunc,
							   BehaviorAction::alloc(treeDef->_action));
	}
}

/**
 * Creates an array of behavior trees created from the provided definitions.
 *
 * Recursively creates each behavior tree from the template provided by the
 * {@link BehaviorNodeDef}. This method will fail if one of the behavior
 * node definition does not define a valid behavior tree.
 *
 * @param treeDefs	An array of definitions for the roots of each behavior
 * tree.
 *
 * @return An array of behavior trees created from the provided definitions.
 */
std::vector<std::shared_ptr<BehaviorNode>> BehaviorManager::createTrees(const std::vector<std::shared_ptr<BehaviorNodeDef>>& treeDefs) {
	std::vector<std::shared_ptr<BehaviorNode>> trees;
	for(auto it = treeDefs.begin(); it != treeDefs.end(); ++it) {
		trees.push_back(createTree((*it)));
	}
	return trees;
}
