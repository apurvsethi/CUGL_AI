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
//  Author: Apurv Sethi and Andrew Matsumoto
//  Version: 5/21/2018
//

#ifndef __CU_BEHAVIOR_ACTION_H__
#define __CU_BEHAVIOR_ACTION_H__

#include <functional>
#include <memory>
#include <string>
#include <cugl/util/CUDebug.h>

namespace cugl {

/**
 * This struct is a reusable definition containing the necessary info to
 * construct a {@link BehaviorAction}. A BehaviorActionDef is used to
 * specify an action for the {@link BehaviorNodeDef} of a {@link LeafNode}.
 */
struct BehaviorActionDef {
	/** The descriptive, identifying name of the action. */
	std::string _name;

	/**
	 * The initialization function to begin running an action, setting up the
	 * action to allow functional updates.
	 */
	std::function<void()> _start;

	/**
	 * The update function processing the action over time.
	 *
	 * This return true if the action is finished and false otherwise.
	 */
	std::function<bool(float dt)> _update;

	/**
	 * The terminate function to interrupt an action that may have been running
	 * over a period of time. This method should ensure that the actor returns
	 * to a stable state when the action is interrupted.
	 */
	std::function<void()> _terminate;

	/**
	 * Creates an uninitialized BehaviorActionDef.
	 *
	 * To create a definition for an action, set the fields for this struct.
	 */
	BehaviorActionDef() : _start(nullptr), _update(nullptr), _terminate(nullptr) {}
};

/**
 * This class provides an action, used to represent actions contained in leaf
 * nodes of behavior trees.
 *
 * An BehaviorAction refers to the action chosen to execute by a behavior
 * tree. Each action is provided to a leaf node of a behavior tree, and is
 * updated during each tick while that leaf node is running.
 *
 * To create a BehaviorAction, you should provide a BehaviorActionDef to the
 * BehaviorNodeDef specifying a leaf node.
 */
class BehaviorAction {
#pragma mark Values
public:
	/** The current state of the action. */
	enum class State : unsigned int {
		/** The action is neither currently running mor has finished running. */
		INACTIVE = 0,
		/** The action is running. */
		RUNNING = 1,
		/** The action is paused (but would be running otherwise). */
		PAUSED = 2,
		/** The action is finished. */
		FINISHED = 3
	};

protected:
	/** The descriptive, identifying name of the action. */
	std::string _name;

	/** The state of the action. */
	BehaviorAction::State _state;

	/**
	 * The initialization function to begin running an action.
	 *
	 * This method is optional to provide.
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
	 *
	 * This method is optional to provide.
	 */
	std::function<void()> _terminate;

#pragma mark -
#pragma mark Constructors
public:
	/**
	 * Creates an uninitialized action.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
	 * the heap, use one of the static constructors instead.
	 */
	BehaviorAction();

	/**
	 * Deletes this action, disposing all resources.
	 */
	~BehaviorAction() { dispose(); }

	/**
	 * Disposes all of the resources used by this action.
	 *
	 *  A disposed action can be safely reinitialized.
	 *
	 * It is unsafe to call this on an action that is still currently
	 * inside of a running behavior tree.
	 */
	void dispose();

	/**
	 * Initializes an action, using the definition as a template.
	 *
	 * @param actionDef	The definition through which this action is
	 * constructed.
	 *
	 * @return true if initialization was successful.
	 */
	bool init(const std::shared_ptr<BehaviorActionDef>& actionDef);

#pragma mark -
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated BehaviorAction, using the definition as a
	 * template.
	 *
	 * @param actionDef The definition through which this action is
	 * constructed.
	 *
	 * @return a newly allocated BehaviorAction specified by the definition.
	 */
	static std::shared_ptr<BehaviorAction> alloc(const std::shared_ptr<BehaviorActionDef>& actionDef) {
		std::shared_ptr<BehaviorAction> result = std::make_shared<BehaviorAction>();
		return (result->init(actionDef) ? result : nullptr);
	}

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

#pragma mark -
#pragma mark Behavior Trees
	/**
	 * Returns the current state of the action.
	 *
	 * This state is used to identify the state of the action.
	 *
	 * @return the state of the action.
	 */
	BehaviorAction::State getState() const { return _state; }

	/**
	 * Begins running the action.
	 *
	 * This method will call the _start function, if one was provided.
	 */
	void start();

	/**
	 * Updates the action.
	 *
	 * Runs an update function, meant to be used on each tick, for the
	 * action, to further process this action. This should only be used
	 * if the action is meant to be running.
	 *
	 * @param dt	The elapsed time since the last frame.
	 *
	 * @return the state of the action after updating.
	 */
	BehaviorAction::State update(float dt);

	/**
	 * Terminates an currently running action.. 
	 * 
	 * This method provided a way to get back to a stable state while in the
	 * middle of running an action.
	 *
	 * You should only call this method on a running action.
	 */
	void terminate();

	/**
	 * Pauses the currently running action. Actions will not be updated while
	 * paused.
	 *
	 * You should only call this method on a running action.
	 */
	void pause();
	
	/**
	 * Resumes the currently paused action. 
	 *
	 * You should only call this method on a paused action.
	 */
	void resume();

	/**
	 * Resets the currently finished action.
	 *
	 * An action can be safely rerun after resetting.
	 *
	 * You should only call this method on a finished action.
	 */
	void reset();

#pragma mark -
#pragma mark Internal Helpers
protected:
	/**
	 * Sets the state of this action.
	 *
	 * This state is used to identify the state of the action.
	 *
	 * @param state The state of this action.
	 */
	void setState(BehaviorAction::State state) { _state = state; }
};


}
#endif /* __CU_BEHAVIOR_ACTION_H__ */
