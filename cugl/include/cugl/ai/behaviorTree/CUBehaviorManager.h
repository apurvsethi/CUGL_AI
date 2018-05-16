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
#include <unordered_map>
#include <cugl/ai/behaviorTree/CUBehaviorNode.h>
#include <cugl/ai/behaviorTree/CUBehaviorAction.h>

namespace cugl {

/**
 * This class provides a behavior manager for behavior trees. It owns,
 * runs, and updates all active behavior trees.
 *
 * Additionally, the behavior manager also acts as a factory for the
 * creation of BehaviorNodes, which are used to represent the behavior trees
 * defined by BehaviorNodeDefs.
 */
class BehaviorManager {
#pragma mark Values
protected:
	/** A map of the trees currently being run by the manager. */
	std::unordered_map<std::string, std::shared_ptr<BehaviorNode>> _trees;

#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized behavior manager.
	 *
	 * This constructor should never be called directly.
	 */
	BehaviorManager() {};

	/**
	 * Deletes this manager, disposing all resources and all trees owned by
	 * this manager.
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
	 * Returns the tree with the given name.
	 *
	 * All trees must be stored with unique names in the BehaviorManager,
	 * and thus there cannot be multiple possible return values.
	 *
	 * @param name	An identifier to find the tree.
	 *
	 * @return the tree with the given name.
	 */
	std::shared_ptr<const BehaviorNode> getTree(const std::string& name) const;

	/**
	 * Returns the state of the tree with the given name.
	 *
	 * All trees must be stored with unique names in the BehaviorManager,
	 * and thus there cannot be multiple possible return values.
	 *
	 * @param name	An identifier to find the tree.
	 *
	 * @return the state of the tree with the given name.
	 */
	BehaviorNode::State getTreeState(const std::string& name) const {
		return getTree(name)->getState();
	}

	/**
	 * Starts running the tree with the given name.
	 *
	 * All trees must be stored with unique names in the BehaviorManager,
	 * and thus there cannot be multiple possible trees to start.
	 *
	 * @param name	An identifier to find the tree.
	 */
	void startTree(const std::string& name);

	/**
	 * Pauses the tree with the given name.
	 *
	 * All trees must be stored with unique names in the BehaviorManager,
	 * and thus there cannot be multiple possible trees to start.
	 *
	 * @param name	An identifier to find the tree.
	 */
	void pauseTree(const std::string& name);

	/**
	 * Resumes running the paused tree with the given name.
	 *
	 * All trees must be stored with unique names in the BehaviorManager,
	 * and thus there cannot be multiple possible trees to start.
	 *
	 * @param name	An identifier to find the tree.
	 */
	void resumeTree(const std::string& name);

	/**
	 * Remove the tree with the given name, if the tree is not currently
	 * running.
	 *
	 * All trees must be stored with unique names in the BehaviorManager,
	 * and thus there cannot be multiple possible return values.
	 *
	 * @param name	An identifier to find the tree.
	 */
	void removeTree(const std::string& name);

	/**
	 * Runs an update function, meant to be used on each tick, for each
	 * behavior tree that is currently running within the manager.
	 *
	 * @param dt	The elapsed time since the last frame.
	 */
	void update(float dt);

	/**
	 * Returns whether BehaviorNode tree was successfully created and added.
	 *
	 * Creates BehaviorNodes from template provided by BehaviorNodeDefs, and
	 * adds it to the BehaviorManager. Returns false if a BehaviorNodeDef
	 * provided does not allow creation of a matching BehaviorNode, or if the
	 * name provided to the treeDef is equal to a name provided for another
	 * tree in the manager. True otherwise.
	 *
	 * @param treeDef	BehaviorNodeDef tree template for a BehaviorNode tree.
	 *
	 * @return whether BehaviorNode tree was successfully created and added.
	 */
	bool addTree(const std::shared_ptr<BehaviorNodeDef>& treeDef);

private:
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
	std::shared_ptr<BehaviorNode> createTree(const std::shared_ptr<BehaviorNodeDef>& treeDef);

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
	std::vector<std::shared_ptr<BehaviorNode>> createTrees(const std::vector<std::shared_ptr<BehaviorNodeDef>>& treeDefs);
};


}
#endif /* __CU_BEHAVIOR_MANAGER_H__ */
