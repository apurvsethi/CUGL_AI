//
//  CUBehaviorManager.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a behavior tree manager.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_BEHAVIOR_MANAGER_H__
#define __CU_BEHAVIOR_MANAGER_H__

#include <functional>
#include <string>
#include <vector>
#include <cugl/ai/behaviorTree/CUBehaviorNode.h>
#include <cugl/ai/behaviorTree/CUBehaviorAction.h>

namespace cugl {
	
/**
 * This class provides a behavior manager for behavior trees. It owns,
 * runs, and updates all active behavior trees.
 *
 * Additionally, the behavior manager also acts as a factory for the
 * creation of actions, which are used to represent the actions taken
 * by leaf nodes in a behavior tree.
 */
class BehaviorManager {
#pragma mark Values
protected:
	/** The trees currently being run by the manager. */
	std::vector<std::shared_ptr<BehaviorNode>> _trees;
	
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized behavior manager.
	 *
	 * This constructor should never be called directly.
	 */
	BehaviorManager();
	
	/**
	 * Deletes this manager, disposing all resources.
	 */
	~BehaviorManager() { dispose(); }
	
	/**
	 * Disposes all of the resources used by this manager.
	 *
	 * It is unsafe to call this while behavior trees are running.
	 */
	void dispose();
	
	/**
	 * Initializes a behavior tree manager.
	 *
	 * @return true if initialization was successful.
	 */
	bool init();
	
#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated BehaviorManager.
	 *
	 * @return a newly allocated BehaviorManager.
	 */
	static std::shared_ptr<BehaviorManager> alloc() {
		std::shared_ptr<BehaviorManager> result = std::make_shared<BehaviorManager>();
		return (result->init() ? result : nullptr);
	}

#pragma mark -
#pragma mark Behavior Trees
	/**
	 * Returns the tree at the given position.
	 *
	 * While trees are enumerated in the order by which they were added,
	 * it is recommended to attempt to retrieve a tree by name instead.
	 *
	 * @param pos   The tree position.
	 *
	 * @return the tree at the given position.
	 */
	const std::shared_ptr<BehaviorNode>& getTree(unsigned int pos) const;
	
	/**
	 * Returns the (first) tree with the given name.
	 *
	 * If there is more than one tree of the given name, it returns the first
	 * one that is found.
	 *
	 * @param name  An identifier to find the tree.
	 *
	 * @return the (first) tree with the given name.
	 */
	const std::shared_ptr<BehaviorNode>& getTreeWithName(const std::string& name) const;
	
	/**
	 * Returns the state of the tree at the given position.
	 *
	 * While trees are enumerated in the order by which they were added,
	 * it is recommended to attempt to retrieve the state by name instead.
	 *
	 * @param pos   The tree position.
	 *
	 * @return the state of the tree at the given position.
	 */
	BehaviorNode::State getTreeState(unsigned int pos) const;
	
	/**
	 * Returns the state of the (first) tree with the given name.
	 *
	 * If there is more than one tree of the given name, it returns the state
	 * of the first one that is found.
	 *
	 * @param name  An identifier to find the tree.
	 *
	 * @return the state of the (first) tree with the given name.
	 */
	BehaviorNode::State getTreeStateWithName(const std::string& name) const;
	
	/**
	 * Add a behavior tree to the manager. This tree cannot be changed once
	 * added, until it is finished running.
	 *
	 * @param tree	The behavior tree to add to this manager
	 */
	void addTree(const std::shared_ptr<BehaviorNodeDef>& tree);
	
	/**
	 * Removes the tree at the given position from the manager, if the
	 * tree is not currently running.
	 *
	 * While trees are enumerated in the order by which they were added,
	 * it is recommended to access a tree by name instead.
	 *
	 * @param pos   The tree position.
	 *
	 * @warning The tree will only be removed if not currently running.
	 */
	void removeTree(unsigned int pos);
	
	/**
	 * Remove the (first) tree with the given name, if the tree is not
	 * currently running.
	 *
	 * If there is more than one tree of the given name, it removes the
	 * first one that is found.
	 *
	 * @param name  An identifier to find the tree.
	 *
	 * @warning The tree will only be removed if not currently running.
	 */
	void removeTreeWithName(const std::string& name);
	
	/**
	 * Starts running the tree at the given position.
	 *
	 * While trees are enumerated in the order by which they were added,
	 * it is recommended to access them by name instead.
	 *
	 * @param pos   The tree position.
	 */
	void startTree(unsigned int pos);
	
	/**
	 * Starts running the (first) tree with the given name.
	 *
	 * If there is more than one tree of the given name, it starts running
	 * the first one that is found.
	 *
	 * @param name  An identifier to find the tree.
	 */
	void startTreeWithName(const std::string& name);
	
	/**
	 * Runs an update function, meant to be used on each tick, for each
	 * behavior tree that is currently running within the manager.
	 */
	void update(float dt);
};
	
	
}
#endif /* __CU_BEHAVIOR_MANAGER_H__ */
