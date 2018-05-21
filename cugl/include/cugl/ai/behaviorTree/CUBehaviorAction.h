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
 * construct a {@link BehaviorAction}.
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
		/** The action is not currently running or has finished running. */
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
	 * It is unsafe to call this on an Action that is still currently
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
	 * @param actionDef Def through which this Action is constructed.
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
	 * Returns a BehaviorAction::State that represents the action's state.
	 *
	 * This state is used to identify the state of the action.
	 *
	 * @return a BehaviorAction::State that represents the action's state.
	 */
	BehaviorAction::State getState() const { return _state; }

	/**
	 * Initializes the action to begin running.
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
	 * Terminates an action, possibly while running. A way to get back
	 * to a stable state while in the middle of running an action.
	 */
	void terminate();

	/**
	 * Pauses the running action. Actions will not be updated while paused.
	 */
	void pause() {
		CUAssertLog(getState() == BehaviorAction::State::RUNNING,
			"Cannot pause an action that is not currently running.");
		setState(BehaviorAction::State::PAUSED);
	}
	
	/**
	 * Resumes a currently paused action.
	 */
	void resume() {
		CUAssertLog(getState() == BehaviorAction::State::PAUSED,
			"Cannot resume a action that is not currently paused.");
		setState(BehaviorAction::State::RUNNING);
	}

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
