//
//  CUBehaviorManager.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a behavior tree manager.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
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
 * Returns the tree with the given name.
 *
 * All trees must be stored with unique names in the BehaviorManager,
 * and thus there cannot be multiple possible return values.
 *
 * @param name	An identifier to find the tree.
 *
 * @return the tree with the given name.
 */
const std::shared_ptr<BehaviorNode>& BehaviorManager::getTree(const std::string& name) const {
	CUAssertLog(_trees.find(name) != _trees.end(),
				"Tree with given name does not exist in BehaviorManager.");
	return _trees.at(name);
}

/**
 * Returns whether BehaviorNode tree was successfully created and added.
 *
 * The name given to a treeDef must be unique amongst the trees added to the
 * BehaviorManager, in order to ensure that trees are accessed properly.
 *
 * Creates BehaviorNodes from template provided by BehaviorNodeDefs, and
 * adds it to the BehaviorManager. Returns false if a BehaviorNodeDef
 * provided does not allow creation of a matching BehaviorNode, true otherwise.
 *
 * @param treeDef	BehaviorNodeDef tree template for a BehaviorNode tree.
 *
 * @return whether BehaviorNode tree was successfully created and added.
 */
bool BehaviorManager::addTree(const std::shared_ptr<BehaviorNodeDef>& treeDef) {
	CUAssertLog(_trees.find(treeDef->_name) == _trees.end(),
				"Tree with given already exists in BehaviorManager.");
	std::shared_ptr<BehaviorNode> tree = createTree(treeDef);
	if (tree == nullptr) {
		return false;
	}
	_trees[tree->getName()] = tree;
	return true;
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
 * Returns BehaviorNode tree created from BehaviorNodeDef tree.
 *
 * Creates BehaviorNodes from template provided by BehaviorNodeDefs, and
 * returns a nullptr if a BehaviorNodeDef provided does not allow creation
 * of a matching BehaviorNode.
 *
 * @param treeDef	BehaviorNodeDef tree template for a BehaviorNode tree.
 *
 * @return BehaviorNode tree created from BehaviorNodeDef tree.
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
		return PriorityNode::alloc(treeDef->_name, treeDef->_priorityFunc,
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
							   treeDef->_action);
	}
}

/**
 * Returns BehaviorNode trees created from BehaviorNodeDef tree templates.
 *
 * Creates BehaviorNodes from templates provided by BehaviorNodeDefs, and
 * returns a nullptr for a BehaviorNode if a BehaviorNodeDef provided does
 * not allow creation of a matching BehaviorNode.
 *
 * @param treeDefs	BehaviorNodeDef tree templates for BehaviorNode trees.
 *
 * @return BehaviorNode trees created from BehaviorNodeDef tree templates.
 */
std::vector<std::shared_ptr<BehaviorNode>> BehaviorManager::createTrees(const std::vector<std::shared_ptr<BehaviorNodeDef>>& treeDefs) {
	std::vector<std::shared_ptr<BehaviorNode>> trees;
	for(auto it = treeDefs.begin(); it != treeDefs.end(); ++it) {
		trees.push_back(createTree((*it)));
	}
	return trees;
}
