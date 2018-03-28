#ifndef __CU_FORMATION_H__
#define __CU_FORMATION_H__

#include <cugl/2d/physics/cu_physics.h>

namespace cugl {

/**
 * This class specifies a formation of obstacle units for path finding.
 * 
 * Formations are built from many bodies, each with a specified relative
 * position and shape from the center of the formation. While the formation
 * is formed, the obstacles within the formation will all have the same velocity.
 * 
 * The formation can be broken, during which the different units will not
 * maintain their relative distances or orientations, and are able to move
 * at different velocities.
 */
class Formation : public ComplexObstacle {
public:
#pragma mark -
#pragma mark Values
public:
    /** The current state of the formation */
    enum class State : unsigned int {
        /** The unit are currently moving into their positions andd orientations. */
        FORMING = 0,
        /** The units are in their specified relative positions and orientations. */
        FORMED = 1,
        /** The units have broken formation. */
        BROKEN = 2,
        /** The units have not been initialized */
        UNINITIALIZED = 3
    };

private:
    /** The center of the formation */
    Vec2 _centroid;

    /** The current state of the formation */
    Formation::State _state;

    /** The individual units of the formation */
    std::vector<std::shared_pointer<Obstacle>> _units;

    /** The target positions of the units relative to the center */
    std::vector<Vec2> _positions;

    /** The target angles for the orientation of each of the units  relative to the center. */
    std::vector<float> _angles;

    /** The orientation angle of this formation. */
    float _orientation;

#pragma mark -
#pragma mark Constructors
    /**
     *  Creates an empty formation. 
     * 
     * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
     * the heap, use one of the static constructors instead. 
     */
    Formation();

    /**
     * Deletes this formation, disposing all resources.
     */
    ~Formation();

    /**
	 * Disposes all of the resources used by this formation.
	 *
	 * A disposed Formation can be safely reinitialized.
	 */
	virtual void dispose();

    /**
     * Initializes an empty formation.
     * 
     * @param centroid the center of this formation.
     * @param orientation the orientation of this formation.
     * 
     * @return true if the formation was successfully initialized, else false.
     */
    virtual bool init(const Vec2& centroid, float orientation);

    /**
     * Initializes a formation with given units.
     * 
     * The target relative positions and orientations of these units are their
     * current relative positions and orientations.
     * 
     * @param units the units for this formation.
     * @param centroid the center of this formation.
     * @param orientation the orientation of this formation.
     * 
     * @return true if the formation was successfully initialized, else false.
     */
    virtual bool initWithUnits(const std::vector<std::shared_pointer<Obstacle>>& units, 
                               const Vec2& centroid, float orientation);

#pragma mark -
#pragma mark Static Constructors

    /**
     * Allocates an empty formation.
     * 
     * @param centroid the center of this formation.
     * @param orientation the orientation of this formation.
     * 
     * @return a newly allocated empty formation.
     */
    static alloc(const Vec2& centroid, float orientation);

    /**
     * Allocates a formation with given units. 
     * 
     * The target relative positions and orientations of these units are their
     * current relative positions and orientations.
     * 
     * @param units the units for this formation.
     * @param centroid the center of this formation.
     * @param orientation the orientation of this formation.
     * 
     * @returns a newly allocated formation.
     */
    static alloc(const std::vector<std::shared_pointer<Obstacle>& units, 
                 const Vec2& centroid, float orientation);

#pragma mark -
#pragma mark Identifiers

    /**
     * Return the center of this formation.
     * 
     * @return the center of this formation 
     */
    const Vec2& getCetroid() const;

    /**
     *  Return the orientation of this formation.
     * 
     * @return the orientation of this formation. 
     */
    float getOrientation() const;

    /**
     * Return the state of this formation.
     * 
     * @return the state of this formation. 
     */
    Formation::State getState() const;

    /**
     *  Return the number of units of this formation.
     * 
     * @return the number of units of this formation. 
     */
    size_t getSize() const;

    /**
     *  Return the units of this formation.
     * 
     * @return the units of this formation. 
     */
    const std::vector<std::shared_pointer<Obstacle>>& getUnits() const;

    /**
     *  Return the velocity of this formation.
     * 
     * @return the velocity of this formation. 
     */
    const Vec2& getVelocity() const;

    /**
     * Sets the y-velocity for this physics body
     *
     * @param value  the y-velocity for this physics body
     */
    virtual float setVY(float value) override;
    
    /**
     * Sets the x-velocity for this physics body
     *
     * @param value  the x-velocity for this physics body
     */
    virtual void setVX(float value) override;

    /**
     * Add a unit to this formation, if it is not already present.
     * 
     * @param unit the unit to add to this formation.
     * 
     * @return whether the unit is added to the formation. 
     */
    bool addUnit(std::shared_pointer<Obstacle>& unit);

    /**
     * Remove a unit from this formation, if it is already present.
     * 
     * @param unit the unit to remove from the string.
     * 
     * @return whether the unit is removed from the formation.
     */
    bool removeUnit(std::shared_pointer<Obstacle>& unit);

    /**
     * Return whether this unit is part of this formation.
     * 
     * @param unit the unit to check for contain.
     * 
     * @return true if the formation contains the unit, else false.
     */
    bool containsUnit(std::shared_pointer<Obstacle>& unit);

#pragma mark -
#pragma mark Forming

    /**
     * Sets the units of the formation to their default relative positions and
     * orientations.
     * 
     * @return whether this formation was formed successfully.
     */
    virtual bool formFormation();

    /**
     * Breaks the formation.
     */
    virtual void breakFormation();

    /**
     * Runs an update function, meant to be used on each tick, for the
	 * formation.
     * 
     * The update method will update the settings of each unit for the
     * formation based on the current state, position and orientation
     * of the formation.
     * 
     * @return the current state of the path finder. 
     */
    virtual Formation::State update() override;

};

}

#endif