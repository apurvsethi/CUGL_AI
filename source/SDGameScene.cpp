//
//  SDGameScene.h
//  Ship Demo
//
//  This is the most important class in this demo.  This class manages the
//  gameplay for this demo.  It is a relativeluy simple class as we are not
//  worried about collisions.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design.  This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together.  However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  Author: Walker White
//  Version: 1/10/17
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "SDGameScene.h"

using namespace cugl;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024

#pragma mark -
#pragma mark Constructors


/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH/dimen.width; // Lock the game to a reasonable resolution
    if (assets == nullptr) {
        return false;
    } else if (!Scene::init(dimen)) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
	_behaviorManager = BehaviorManager::alloc();
    _input.init();
    
    // Acquire the scene built by the asset loader and resize it the scene
    auto scene = _assets->get<Node>("game");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    
    // Get the scene components.
    _allSpace   = _assets->get<Node>("game_field");
	_shipNode   = _assets->get<Node>("game_field_player");

    // Create the ship and planets
	auto resources  = std::dynamic_pointer_cast<Label>(_assets->get<Node>("game_field_player_resources"));
    _shipModel = ResourceContainer::alloc(_shipNode->getPosition(), 0, resources);

	auto planet = _assets->get<Node>("game_field_near_planetA");
	resources = std::dynamic_pointer_cast<Label>(_assets->get<Node>("game_field_near_planetA_resources"));
	_homePlanet = ResourceContainer::alloc(planet->getPosition(), 0, resources);

	auto planetButton = std::dynamic_pointer_cast<Button>(_assets->get<Node>("game_field_near_planetB"));
	planetButton->setListener([=] (const std::string& name, bool down) {
		if (down) {
			_planetA->setNumResources(_planetA->getNumResources() + 1);
		}
	});
	resources = std::dynamic_pointer_cast<Label>(_assets->get<Node>("game_field_near_planetB_resources"));
	_planetA = ResourceContainer::alloc(planetButton->getPosition(), 3, resources);
	planetButton->activate(2);

	planetButton = std::dynamic_pointer_cast<Button>(_assets->get<Node>("game_field_near_planetC"));
	planetButton->setListener([=] (const std::string& name, bool down) {
		if (down) {
			_planetB->setNumResources(_planetB->getNumResources() + 1);
		}
	});
	resources = std::dynamic_pointer_cast<Label>(_assets->get<Node>("game_field_near_planetC_resources"));
	_planetB = ResourceContainer::alloc(planetButton->getPosition(), 3, resources);
	planetButton->activate(3);

    addChild(scene);

	std::shared_ptr<BehaviorNodeDef> behaviorNodeDef = setupBehaviorTree();
	_behaviorManager->addTree(behaviorNodeDef);
	_behaviorManager->startTree(behaviorNodeDef->_name);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
		std::dynamic_pointer_cast<Button>(_assets->get<Node>("game_field_near_planetB"))->deactivate();
		std::dynamic_pointer_cast<Button>(_assets->get<Node>("game_field_near_planetC"))->deactivate();
        removeAllChildren();
        _input.dispose();
        _allSpace = nullptr;
        _shipNode = nullptr;
        _shipModel = nullptr;
        _active = false;
    }
}


#pragma mark -
#pragma mark Gameplay Handling

std::shared_ptr<BehaviorNodeDef> GameScene::setupBehaviorTree() {
	std::shared_ptr<BehaviorNodeDef> behaviorNodeDef = BehaviorParser::parseFile("json/behaviorTree.json");
	std::shared_ptr<BehaviorNodeDef> leafNode = behaviorNodeDef->getNodeByName("DropResources");
	leafNode->_priorityFunc = [=] () -> float {
		return _shipModel->getPosition().distance(_homePlanet->getPosition()) < 60.0f && _shipModel->getNumResources() > 0;
	};
	std::shared_ptr<BehaviorActionDef> action = std::make_shared<BehaviorActionDef>();
	action->_name = "DropResources";
	action->_update = exchangeResources(_shipModel, _homePlanet);
	leafNode->_action = action;

	leafNode = behaviorNodeDef->getNodeByName("PickResourcesA");
	leafNode->_priorityFunc = [=] () -> float {
		return _shipModel->getPosition().distance(_planetA->getPosition()) < 60.0f && _shipModel->getNumResources() < 2 && _planetA->getNumResources() > 0;
	};
	action = std::make_shared<BehaviorActionDef>();
	action->_name = "PickResourcesA";
	action->_update = exchangeResources(_planetA, _shipModel);
	leafNode->_action = action;

	leafNode = behaviorNodeDef->getNodeByName("PickResourcesB");
	leafNode->_priorityFunc = [=] () -> float {
		if (_shipModel->getPosition().distance(_planetB->getPosition()) < 60.0f && _shipModel->getNumResources() < 2 && _planetB->getNumResources() > 0) {
			return 1.0f;
		}
		return 0.0f;
	};
	action = std::make_shared<BehaviorActionDef>();
	action->_name = "PickResourcesB";
	action->_update = exchangeResources(_planetB, _shipModel);
	leafNode->_action = action;

	leafNode = behaviorNodeDef->getNodeByName("ToHome");
	leafNode->_priorityFunc = [=] () -> float {
		if (_shipModel->getNumResources() == 2 || (_planetA->getNumResources() == 0 && _planetB->getNumResources() == 0)) {
			return 0.9f;
		}
		return 0.0f;
	};
	action = std::make_shared<BehaviorActionDef>();
	action->_name = "ToHome";
	action->_update = move(_homePlanet->getPosition());
	leafNode->_action = action;

	leafNode = behaviorNodeDef->getNodeByName("ToPlanetA");
	leafNode->_priorityFunc = [=] () -> float {
		if (_shipModel->getNumResources() == 2 || _planetA->getNumResources() == 0) {
			return 0.0f;
		}
		float priority = 0.1f;
		if (_shipModel->getPosition().distance(_planetA->getPosition()) < _shipModel->getPosition().distance(_planetB->getPosition())) {
			priority *= 2;
		};
		if (_planetA->getNumResources() >= 2) {
			priority *= 2;
		}
		return priority;
	};
	action = std::make_shared<BehaviorActionDef>();
	action->_name = "ToPlanetA";
	action->_update = move(_planetA->getPosition());
	leafNode->_action = action;

	leafNode = behaviorNodeDef->getNodeByName("ToPlanetB");
	leafNode->_priorityFunc = [=] () -> float {
		if (_shipModel->getNumResources() == 2 || _planetB->getNumResources() == 0) {
			return 0.0f;
		}
		float priority = 0.1f;
		if (_shipModel->getPosition().distance(_planetB->getPosition()) < _shipModel->getPosition().distance(_planetA->getPosition())) {
			priority *= 2;
		};
		if (_planetB->getNumResources() >= 2) {
			priority *= 2;
		}
		return priority;
	};
	action = std::make_shared<BehaviorActionDef>();
	action->_name = "ToPlanetB";
	action->_update = move(_planetB->getPosition());
	leafNode->_action = action;

	return behaviorNodeDef;
}

std::function<bool(float)> GameScene::exchangeResources(const std::shared_ptr<ResourceContainer> from,
											 const std::shared_ptr<ResourceContainer> to) {
	return ([=] (float dt) -> bool {
		from->setNumResources(from->getNumResources() - 1);
		to->setNumResources(to->getNumResources() + 1);
		return true;
	});
}

std::function<bool(float)> GameScene::move(Vec2 pos) {
	return ([=] (float dt) -> bool {
		Vec2 endPos = pos - _shipModel->getPosition();
		endPos.normalize();
		_shipModel->getPosition() += endPos;
		_shipNode->setPosition(_shipModel->getPosition());
		return pos.distance(_shipModel->getPosition()) < 60.0f;
	});
}

/**
 * Resets the status of the game so that we can play again.
 */
void GameScene::reset() {
    // Reset the ships and input
	_shipModel->reset();
	_homePlanet->reset();
	_planetA->reset();
	_planetB->reset();
    _input.clear();
}

/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameScene::update(float timestep) {
    _input.update(timestep);
    
    // Reset the game if necessary
    if (_input.didReset()) { reset(); }

	if (_behaviorManager->getTreeState("ShipBehavior") == BehaviorNode::State::FINISHED) {
		_behaviorManager->restartTree("ShipBehavior");
	}
	_behaviorManager->update(timestep);
}
