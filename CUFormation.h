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

    /** The target angles for the orientation of each of the units */
    std::vector<float> _angles;

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
     * 
     */

#pragma mark -
#pragma mark Scene Graph Internals
    /**
     * Creates the outline of the physics fixtures in the debug wireframe
     *
     * The debug wireframe is use to outline the fixtures attached to this object.
     * This is very useful when the fixtures have a very different shape than
     * the texture (e.g. a circular shape attached to a square texture).
     */
    virtual void resetDebug() override;
    
    /**
     * Repositions the debug wireframe so that it agrees with the physics object.
     *
     * The debug wireframe is use to outline the fixtures attached to this object.
     * This is very useful when the fixtures have a very different shape than
     * the texture (e.g. a circular shape attached to a square texture).
     */
    virtual void updateDebug() override;

};

}

#endif