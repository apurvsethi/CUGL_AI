//
//  CUSelectorNode.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a selector composite behavior node.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#include <cugl/ai/behaviorTree/CUSelectorNode.h>

using namespace cugl;

#pragma mark -
#pragma mark Behavior Tree
/**
 * Returns the BehaviorNode::State of the selector node.
 *
 * Runs an update function, meant to be used on each tick, for the
 * selector node (and all nodes below this node in the tree).
 * The state for this node is derived from the state of the running
 * or most recently run node.
 *
 * The priority value of the node is updated within this function, based
 * on the priority values of the nodes below the given node.
 *
 * @param dt	The elapsed time since the last frame.
 *
 * @return the BehaviorNode::State of the selector node.
 */
BehaviorNode::State SelectorNode::update(float dt) {
        if (_state == BehaviorNode::State::RUNNING) {
        std::shared_ptr<BehaviorNode> activeChild;
        for (auto it = _children.begin(); it != _children.end(); ++it) {
            (*it)->update(dt);
            switch((*it)->_state) {
                case BehaviorNode::State::RUNNING:
                    activeChild = *it;
                    break;
                case BehaviorNode::State::FINISEHD;
                    activeChild = *it;
                    _state = BehaviorNode::State::FINISHED;
                    break;
            }
        }
        if (_preempt && _state == BehaviorNode::State::RUNNING) {
            activeChild->_state = BehaviorNode::State::UNINITIALIZED;
            activeChild = nullptr;
        }
        if (!activeChild) {
            activeChild = _children[0];
            activeChild->_state = BehaviorNode::State::RUNNING;
        }
        _priority = _priorityFunc ? _priorityFunc() : activeChild->_priority;
    }
    return _state;
}