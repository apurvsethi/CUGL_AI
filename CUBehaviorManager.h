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
#include <cugl/ai/behaviorTree/CUAction.h>

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
private:
	/** This macro disables the copy constructor (not allowed on assets) */
	CU_DISALLOW_COPY_AND_ASSIGN(BehaviorManager);

#pragma mark Values
protected:
	/** The trees currently being run by the manager. */
	std::vector<std::shared_ptr<BehaviorNode>> _trees
	
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
	 * Add a behavior tree to the manager. This tree cannot be changed once added,
	 * until it is finished running.
	 */
	void addTree(std::shared_ptr<BehaviorNode>);
	
};
	
	
}
#endif /* __CU_BEHAVIOR_MANAGER_H__ */
