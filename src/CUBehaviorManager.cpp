//
//  CUBehaviorManager.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a behavior tree manager.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

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
 * Creates an uninitialized behavior manager.
 *
 * This constructor should never be called directly.
 */
BehaviorManager::BehaviorManager() {}


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
	return true;
}

#pragma mark -
#pragma mark Behavior Trees
/**
 * Returns the tree at the given position.
 *
 * While trees are enumerated in the order by which they were added,
 * it is recommended to attempt to retrieve a tree by name instead.
 *
 * @param pos	The tree position.
 *
 * @return the tree at the given position.
 */
const std::shared_ptr<BehaviorNode>& BehaviorManager::getTree(unsigned int pos) const {
	CUAssertLog(pos < _trees.size(), "Position index out of bounds");
	return _trees[pos];
}

/**
 * Returns the (first) tree with the given name.
 *
 * If there is more than one tree of the given name, it returns the first
 * one that is found.
 *
 * @param name	An identifier to find the tree.
 *
 * @return the (first) tree with the given name.
 */
const std::shared_ptr<BehaviorNode>& BehaviorManager::getTreeWithName(const std::string& name) const {
	for(auto it = _trees.begin(); it != _trees.end(); ++it) {
		if ((*it)->getName() == name) {
			return *it;
		}
	}
	return nullptr;
}

/**
 * Returns the state of the tree at the given position.
 *
 * While trees are enumerated in the order by which they were added,
 * it is recommended to attempt to retrieve the state by name instead.
 *
 * @param pos	The tree position.
 *
 * @return the state of the tree at the given position.
 */
BehaviorNode::State BehaviorManager::getTreeState(unsigned int pos) const {
	CUAssertLog(pos < _trees.size(), "Position index out of bounds");
	return _trees[pos]->getState();
}

/**
 * Returns the state of the (first) tree with the given name.
 *
 * If there is more than one tree of the given name, it returns the state
 * of the first one that is found.
 *
 * @param name	An identifier to find the tree.
 *
 * @return the state of the (first) tree with the given name.
 */
BehaviorNode::State BehaviorManager::getTreeStateWithName(const std::string& name) const {
	for(auto it = _trees.begin(); it != _trees.end(); ++it) {
		if ((*it)->getName() == name) {
			return (*it)->getState();
		}
	}
	return BehaviorNode::State::UNINITIALIZED;
}

/**
 * Returns whether BehaviorNode tree was successfully created and added.
 *
 * Creates BehaviorNodes from template provided by BehaviorNodeDefs, and
 * adds it to the BehaviorManager. Returns falsereturns if a BehaviorNodeDef
 * provided does not allow creation of a matching BehaviorNode, true
 * otherwise.
 *
 * @param treeDef	BehaviorNodeDef tree template for a BehaviorNode tree.
 *
 * @return whether BehaviorNode tree was successfully created and added.
 */
bool BehaviorManager::addTree(const std::shared_ptr<BehaviorNodeDef>& treeDef) {
	std::shared_ptr<BehaviorNode> tree = createTree(treeDef);
	if (tree == nullptr) {
		return false;
	}
	_trees.push_back(tree);
	return true;
}

/**
 * Removes the tree at the given position from the manager, if the
 * tree is not currently running.
 *
 * While trees are enumerated in the order by which they were added,
 * it is recommended to access a tree by name instead.
 *
 * @param pos	The tree position.
 *
 * @warning The tree will only be removed if not currently running.
 */
void BehaviorManager::removeTree(unsigned int pos) {
	CUAssertLog(pos < _trees.size(), "Position index out of bounds");
	CUAssertLog(_trees[pos]->getState() == BehaviorNode::State::RUNNING,
				"Tree is currently running");
	std::shared_ptr<BehaviorNode> child = _trees[pos];
	for(int ii = pos; ii < _trees.size() - 1; ii++) {
		_trees[ii] = _trees[ii + 1];
	}
	_trees.resize(_trees.size() - 1);
}

/**
 * Remove the (first) tree with the given name, if the tree is not
 * currently running.
 *
 * If there is more than one tree of the given name, it removes the
 * first one that is found.
 *
 * @param name	An identifier to find the tree.
 *
 * @warning The tree will only be removed if not currently running.
 */
void BehaviorManager::removeTreeWithName(const std::string& name) {
	for(int ii = 0; ii < _trees.size(); ii++) {
		if (_trees[ii]->getName() == name) {
			removeTree(ii);
			break;
		}
	}
}

/**
 * Starts running the tree at the given position.
 *
 * While trees are enumerated in the order by which they were added,
 * it is recommended to access them by name instead.
 *
 * @param pos	The tree position.
 */
void BehaviorManager::startTree(unsigned int pos) {
	CUAssertLog(pos < _trees.size(), "Position index out of bounds");
	_trees[pos]->start();
}

/**
 * Starts running the (first) tree with the given name.
 *
 * If there is more than one tree of the given name, it starts running
 * the first one that is found.
 *
 * @param name	An identifier to find the tree.
 */
void BehaviorManager::startTreeWithName(const std::string& name) {
	for(auto it = _trees.begin(); it != _trees.end(); ++it) {
		if ((*it)->getName() == name) {
			(*it)->start();
		}
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
		if ((*it)->getState() == BehaviorNode::State::RUNNING) {
			(*it)->update(dt);
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
	for (std::shared_ptr<BehaviorNodeDef> treeDef : treeDefs) {
		trees.push_back(createTree(treeDef));
	}
	return trees;
}
