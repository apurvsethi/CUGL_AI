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

#include <cugl/ai/behaviorTree/CUBehaviorAction.h>

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Creates an uninitialized action.
 *
 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
 * the heap, use one of the static constructors instead.
 */
BehaviorAction::BehaviorAction() :
_name(""),
_state(BehaviorAction::State::INACTIVE),
_start(nullptr),
_update(nullptr),
_terminate(nullptr) {};

/**
 * Disposes all of the resources used by this action.
 *
 * It is unsafe to call this on an Action that is still currently
 * inside of a running behavior tree.
 */
void BehaviorAction::dispose() {
	_name = "";
	_state = BehaviorAction::State::INACTIVE;
	_start = nullptr;
	_update = nullptr;
	_terminate = nullptr;
}

/**
 * Initializes an action, using the definition as a template.
 *
 * @param actionDef	The definition through which this action is
 * constructed.
 *
 * @return true if initialization was successful.
 */
bool BehaviorAction::init(const std::shared_ptr<BehaviorActionDef>& actionDef) {
	CUAssertLog(actionDef, "Must provided a non-null action definition.");
	CUAssertLog(actionDef->_update, "Must provided an update function in the def.");
	_name = actionDef->_name;
	setState(BehaviorAction::State::INACTIVE);
	_start = actionDef->_start;
	_update = actionDef->_update;
	_terminate = actionDef->_terminate;
	return true;
}

#pragma mark -
#pragma mark Behavior Trees
/**
* Initializes the action to begin running.
*/
void BehaviorAction::start() {
	CUAssertLog(getState() == BehaviorAction::State::INACTIVE, "Must start an inactive action");
	setState(BehaviorAction::State::RUNNING);
	if (_start) {
		_start();
	}
}

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
BehaviorAction::State BehaviorAction::update(float dt) {
	if (getState() == BehaviorAction::State::RUNNING) {
		setState(_update(dt) ? BehaviorAction::State::FINISHED
			: BehaviorAction::State::RUNNING);
	}
	return getState();
}
