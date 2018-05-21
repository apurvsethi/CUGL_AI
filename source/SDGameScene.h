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
//  Version: 1/10/18
//
#ifndef __SD_GAME_SCENE_H__
#define __SD_GAME_SCENE_H__
#include <cugl/cugl.h>
#include <vector>
#include "SDResourceContainer.h"
#include "SDInput.h"


/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public cugl::Scene {
protected:
    /** The asset manager for this game mode. */
	std::shared_ptr<cugl::AssetManager> _assets;
	/** The behavior tree manager for this game mode. */
	std::shared_ptr<cugl::BehaviorManager> _behaviorManager;

    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    ShipInput _input;
    
    // VIEW
    /** Filmstrip representing the animated ship */
    std::shared_ptr<cugl::Node> _shipNode;
    /** Node to hold all of our graphics. Necesary for resolution indepedence. */
    std::shared_ptr<cugl::Node> _allSpace;

    // MODEL
    // A page-out could dispose of the view as long as it just has this.
    /** The model of the ship */
	std::shared_ptr<ResourceContainer>  _shipModel;
	/** The model of the home planet */
	std::shared_ptr<ResourceContainer>  _homePlanet;
	/** The model of resource planet a */
	std::shared_ptr<ResourceContainer>  _planetA;
	/** The model of resource planet b */
	std::shared_ptr<ResourceContainer>  _planetB;

    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    GameScene() : Scene() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    
#pragma mark -
#pragma mark Gameplay Handling

	/** Function to setup behavior node def from json */
	std::shared_ptr<cugl::BehaviorNodeDef> setupBehaviorTree();

	/** Function returning BehaviorAction update for exchanging resources */
	std::function<bool(float)> exchangeResources(const std::shared_ptr<ResourceContainer> from,
														const std::shared_ptr<ResourceContainer> to);

	/** Function return BehaviorAction update for ship moving */
	std::function<bool(float)> move(cugl::Vec2 pos);

    /**
     * The method called to update the game mode.
     *
     * This method contains any gameplay code that is not an OpenGL call.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);

    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();

};

#endif /* __SD_GAME_SCENE_H__ */
