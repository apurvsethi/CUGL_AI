//
//  CUBehaviorManager.h
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
//  Version: 5/22/2018
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
 * This class provides a manager for behavior trees. It owns, runs, and updates
 * all active behavior trees. You should always use a BehaviorManager to create
 * behavior trees, and you should never use a behavior tree not owned by a
 * BehaviorManager.
 *
 * To create a behavior tree, the manager uses a {@link BehaviorNodeDef} for
 * the root node, and constructs the behavior tree defined by that definition.
 *
 * Each update frame, the behavior manager updates all running behavior trees
 * until they are finished. The behavior manager can pause, reset or restart
 * any behavior tree it owns.
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
	 * Returns whether this manager contains a tree with the given name.
	 *
	 * @param name	An identifier to find the tree.
	 * 
	 * @return whether this manager contains a tree with the given name.
	 */
	bool containsTree(const std::string& name) const;

	/**
	 * Returns a (weak) reference to the behavior tree with the given name.
	 *
	 * All trees must be stored with a unique names in the BehaviorManager,
	 * and thus there cannot be multiple possible return values.
	 *
	 * As a weak reference, this manager does not pass ownership of the tree.
	 *
	 * @param name	An identifier to find the tree.
	 *
	 * @return a (weak) reference to the tree with the given name.
	 */
	const BehaviorNode* getTree(const std::string& name) const;

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
	 * Pauses the running tree with the given name.
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
	 * Restarts the tree with the given name, if the tree is finished.
	 *
	 * All trees must be stored with unique names in the BehaviorManager,
	 * and thus there cannot be multiple possible return values.
	 *
	 * @param name	An identifier to find the tree.
	 */
	void restartTree(const std::string& name);

	/**
	 * Runs an update function, meant to be used on each tick, for each
	 * behavior tree that is currently running within the manager.
	 *
	 * @param dt	The elapsed time since the last frame.
	 */
	void update(float dt);

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
	bool addTree(const std::shared_ptr<BehaviorNodeDef>& treeDef);

private:
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
	std::shared_ptr<BehaviorNode> createTree(const std::shared_ptr<BehaviorNodeDef>& treeDef);

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
	std::vector<std::shared_ptr<BehaviorNode>> createTrees(const std::vector<std::shared_ptr<BehaviorNodeDef>>& treeDefs);
};


}
#endif /* __CU_BEHAVIOR_MANAGER_H__ */
