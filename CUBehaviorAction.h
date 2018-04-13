//
//  CUBehaviorAction.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for an action, used as part of a behavior tree.
//
//  You should never directly instantiate an object of this class.  Instead, you
//  should create an action in a factory format, using the BehaviorTreeManager
//  class.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#ifndef __CU_BEHAVIOR_ACTION_H__
#define __CU_BEHAVIOR_ACTION_H__

#include <functional>
#include <string>

namespace cugl {

/**
 * A def through which an action is constructed; a template to use in order to create
 * an action, using BehaviorManager.createAction.
 */
struct BehaviorActionDef {
	/**
	 * The initialization function to begin running an action.
	 */
	std::function<void()> _start;
	
	/**
	 * The update function processing the action over time.
	 *
	 * This return true if the action is finished and false otherwise.
	 */
	std::function<bool(float dt)> _update;
	
	/**
	 * The terminate function to interrupt an action over time.
	 *
	 * This return true if the action is finished and false otherwise.
	 */
	std::function<void()> _terminate;
	
	BehaviorActionDef() : _start(nullptr), _update(nullptr), _terminate(nullptr) {}
};
	
/**
 * This class provides an action, used to represent actions in leaf nodes
 * of behavior trees.
 *
 * An action refers to behavior followed by a behavior tree when a leaf node
 * is being run, with updates run over time.
 */
class BehaviorAction {
#pragma mark Values
public:
	/** The current state of the action. */
	enum class State : unsigned int {
		/** The action has not yet been run. */
		UNINITIALIZED = 0,
		/** The action is running. */
		RUNNING = 1,
		/** The action is finished. */
		FINISHED = 2
	};

protected:
	/** The descriptive, identifying name of the action. */
	std::string _name;
	
	/** The state of the action. */
	BehaviorAction::State _state;
	
	/**
	 * The initialization function to begin running an action.
	 */
	std::function<void()> _start;
	
	/**
	 * The update function processing the action over time.
	 *
	 * This return true if the action is finished and false otherwise.
	 */
	std::function<bool(float dt)> _update;
	
	/**
	 * The terminate function to interrupt an action over time.
	 *
	 * This return true if the action is finished and false otherwise.
	 */
	std::function<void()> _terminate;
	
#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized action.
	 *
	 * This constructor should never be called directly.
	 */
	BehaviorAction();
	
	/**
	 * Deletes this action, disposing all resources.
	 */
	~BehaviorAction() { dispose(); }
	
	/**
	 * Disposes all of the resources used by this action.
	 *
	 * It is unsafe to call this on an Action that is still currently
	 * inside of a running behavior tree.
	 */
	void dispose();
	
	/**
	 * Initializes an action with the given name.
	 *
	 * @param name 		The name of the action.
	 * @param actionDef Def through which this Action is constructed.
	 *
	 * @return true if initialization was successful.
	 */
	bool init(const std::string& name,
			  const std::shared_ptr<BehaviorActionDef>& actionDef);
	
#pragma mark -
#pragma mark Identifiers
	/**
	 * Returns a string that is used to identify the action.
	 *
	 * This name is used to identify actions in a behavior tree.
	 *
	 * @return a string that is used to identify the action.
	 */
	const std::string& getName() const { return _name; }
	
	/**
	 * Sets a string that is used to identify the action.
	 *
	 * This name is used to identify actions in a behavior tree.
	 *
	 * @param name  A string that is used to identify the action.
	 */
	void setName(const std::string& name) { _name = name; }
	
#pragma mark -
#pragma mark Behavior Trees
	/**
	 * Returns a BehaviorAction::State that represents the action's state.
	 *
	 * This state is used to identify the state of the action.
	 *
	 * @return a BehaviorAction::State that represents the action's state.
	 */
	BehaviorAction::State getState() const { return _state; }
	
	/**
	 * Initializes the action to make it begin running.
	 */
	void start();
	
	/**
	 * Returns the BehaviorAction::State of the action.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * action, to further process this action.
	 *
	 * @param dt	The timestep over which the update needs to process.
	 *
	 * @return the BehaviorAction::State of the action.
	 */
	BehaviorAction::State update(float dt);
	
	/**
	 * Terminates the action, perhaps while it is running. A way to get back
	 * to a stable state while in the middle of running an action.
	 */
	void terminate();
	
private:
#pragma mark -
#pragma mark Internal Helpers
	/**
	 * Sets the state of the action.
	 *
	 * @param state    BehaviorAction::State of the action.
	 */
	void setState(BehaviorAction::State state) { _state = state; }
};
	
	
}
#endif /* __CU_BEHAVIOR_ACTION_H__ */
