//
//  SDResourceContainer.h
//  BehaviorTree Demo
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
//  Author: Walker White
//  Version: 1/10/17
//
#ifndef __SD_RESOURCE_CONTAINER_H__
#define __SD_RESOURCE_CONTAINER_H__
#include <cugl/cugl.h>


#pragma mark -
#pragma mark Resource Container

/**
 * Player avatar for the ship demo.
 *
 * All positional information about the ship goes in this class and not
 * in the sprite node.  That is because we are not animating the ship
 * (other than using the filmstrip to show banking).  We are animating
 * the background.  This forces us to decouple the model from the sprite.
 *
 * You should ALWAYS do this. If you do not do this, I will be most
 * displeased with you.
 */
class ResourceContainer {
private:
	/** This macro disables the copy constructor (not allowed on models) */
	CU_DISALLOW_COPY_AND_ASSIGN(ResourceContainer);

protected:
	/** Initial osition of the container in world space */
	cugl::Vec2    _initialPos;
	/** Position of the container in world space */
	cugl::Vec2    _position;
	/** Initial number of resource available in this container */
	unsigned int _initialResources;
	/** Number of resource available in this container */
	unsigned int _resources;
	/** The label for the number of resources this container has */
	std::shared_ptr<cugl::Label> _resourcesSprite;

public:
#pragma mark Constructors
	/*
	 * Creates a new container at the origin.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate a model on
	 * the heap, use one of the static constructors instead.
	 */
	ResourceContainer(void) {}

	/**
	 * Destroys this container, releasing all resources.
	 */
	~ResourceContainer(void) { dispose(); }

	/**
	 * Disposes all resources and assets of this container
	 *
	 * Any assets owned by this object will be immediately released.  Once
	 * disposed, a container may not be used until it is initialized again.
	 */
	void dispose();

	/**
	 * Initializes a new container at the origin with 0 resources.
	 *
	 * An initializer does the real work that the constructor does not.  It
	 * initializes all assets and makes the object read for use.  By separating
	 * them, we allow ourselfs non-pointer references to complex objects.
	 *
	 * @return true if the model is initialized properly, false otherwise.
	 */
	virtual bool init() { return init(cugl::Vec2::ZERO, 0, nullptr); }

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
	virtual bool init(const cugl::Vec2& pos, unsigned int resources,
					  const std::shared_ptr<cugl::Label> resourcesSprite);


#pragma mark Static Constructors
	/**
	 * Returns a newly allocated container at the origin with 0 resources.
	 *
	 * This is a static constructor. You call it with the ResourceContainer::alloc().
	 * We prefer static constructors as they make the usage of shared pointers
	 * much simpler (and prevent the temptation of making a weak pointer on
	 * the heap).
	 *
	 * @return a newly allocated container at the origin.
	 */
	static std::shared_ptr<ResourceContainer> alloc() {
		std::shared_ptr<ResourceContainer> result = std::make_shared<ResourceContainer>();
		return (result->init() ? result : nullptr);
	}

	/**
	 * Returns a newly allocated container with the given sprite, position, and
	 * resources.
	 *
	 * This is a static constructor. You call it with the ResourceContainer::alloc().
	 * We prefer static constructors as they make the usage of shared pointers
	 * much simpler (and prevent the temptation of making a weak pointer on
	 * the heap).
	 *
	 * @param pos   			Initial position in world coordinates
	 * @param resources			The initial number of resources in this container
	 * @param resourcesSprite	The label for resources of this container
	 *
	 * @return a newly allocated ship at the given position and resources.
	 */
	static std::shared_ptr<ResourceContainer> alloc(const cugl::Vec2& pos, unsigned int resources,
											  const std::shared_ptr<cugl::Label> resourcesSprite) {
		std::shared_ptr<ResourceContainer> result = std::make_shared<ResourceContainer>();
		return (result->init(pos, resources, resourcesSprite) ? result : nullptr);
	}


#pragma mark -
#pragma mark Accessors
	// SHORT METHODS CAN BE IN-LINED IN C++

	/**
	 * Returns the position as a reference.
	 *
	 * This allows us to modify the value.
	 *
	 * @return the position as a reference.
	 */
	cugl::Vec2& getPosition() { return _position; }

	/**
	 * Returns the current number of resources in this container
	 *
	 * @return the current number of resources in this container
	 */
	unsigned int getNumResources() { return _resources; }

	/**
	 * Sets the current number of resources in this container
	 *
	 * @param resources	The number of resources in this container
	 */
	void setNumResources(unsigned int resources) {
		_resources = resources;
		_resourcesSprite->setText(std::to_string(resources));
	}

#pragma mark -
#pragma mark Game
	/** Reset container state to initial state */
	void reset();

};

#endif /* __SD_RESOURCE_CONTAINER_H__ */
