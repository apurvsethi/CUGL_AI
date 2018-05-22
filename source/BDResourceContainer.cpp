//
//  BDResourceContainer.cpp
//  BehaiorTree Demo
//
//  This model encapsulates all of the information for the ship demo. As there
//  are no collisions in ship demo, this class is pretty simple.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design.  This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together.  However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  Pay close attention to how this class designed. This class uses our standard
//  shared-pointer architecture which is common to the entire engine.
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
//  Note that this object manages its own texture, but DOES NOT manage its own
//  scene graph node.  This is a very common way that we will approach complex
//  objects.
//
//  Author: Apurv Sethi and Andrew Matsumoto
//  Version: 5/22/2018
//
#include "BDResourceContainer.h"


using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new container with the given sprite, position, and resources.
 *
 * An initializer does the real work that the constructor does not.  It
 * initializes all assets and makes the object read for use.  By separating
 * them, we allow ourselfs non-pointer references to complex objects.
 *
 * @param pos   			Initial position in world coordinates
 * @param resources			The initial number of resources in this container
 * @param resourcesSprite	The label for resources of this container
 *
 * @return true if the model is initialized properly, false otherwise.
 */
bool ResourceContainer::init(const cugl::Vec2& pos, unsigned int resources,
					   const std::shared_ptr<cugl::Label> resourcesSprite) {
	_initialPos = pos;
	_position = pos;
	_initialResources = resources;
	_resourcesSprite = resourcesSprite;
	setNumResources(resources);
	return true;
}

/**
 * Disposes all resources and assets of this container
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a container may not be used until it is initialized again.
 */
void ResourceContainer::dispose() {
	_resourcesSprite = nullptr;
}

#pragma mark -
#pragma mark Accessors
/**
 * Sets the current number of resources in this container
 *
 * @param resources	The number of resources in this container
 */
void ResourceContainer::setNumResources(unsigned int resources) {
	_resources = resources;
	_resourcesSprite->setText(std::to_string(resources));
}

#pragma mark -
#pragma mark Game
/** Reset container state to initial state */
void ResourceContainer::reset() {
	_position = _initialPos;
	setNumResources(_initialResources);
}


