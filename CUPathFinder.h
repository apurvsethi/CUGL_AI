#ifndef __CU_Path_Finder_H__
#define __CU_Path_Finder_H__

#include "CUGrid.h"

namespace cugl {

typedef std::function<float(GridNode, GridNode)> HeuristicFunction;

/**
 * This class provides a path finder for a given agent and world.
 * 
 * A path finder will compute the optimal path for an agent to
 * a target location using an algorithm specific to each type
 * of path finder and move the agent along the computed path.
 * A path finder must use update to run on each tick, to update
 * its optimal path and to the agent along that path. 
 */
class PathFinder {
#pragma mark Values
public:
    /** The current state of the path finder. */
    enum class State : unsigned int {
        /** The agent successfully moved to the target path. */
        SUCCESS = 0,
        /** The agent failed to move to the target path. */
        FAILURE = 1,
        /** The agent is currently moving to the target path. */
        RUNNING = 2,
        /** The path finder has not yet been initialized. */
        UNINITIALIZED = 3,
    };
protected:
    /** The grid to run the pathfinding algorithm for */
    Grid* _grid;

    /** The pathfinding agent */
    std::shared_pointer<Obstacle> _agent;

    /** The world to run pathfinding in */
    std::shared_pointer<ObstacleWorld> _world;

    /** The heuristic function for scoring the grid nodes. */
    HeuristicFunction _h;

    /** Whether to perform smoothing on the path. */
    bool _smoothing;

    /** The computed optimal path. */
    std::vector<Vec2> _path;

    /** The state of the path finder. */
    PathFinder::State _state;
public:

#pragma mark Constructors
    /**
     * Creates a new path finder.
     *
     * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
     * the heap, use one of the static constructors instead.
     */
    PathFinder();

    /**
     * Deletes this path finder, disposing all resources.
     */
    virtual ~PathFinder() { dispose(); }

    /**
     * Disposes all of the resources used by this path finder.
     *
     * A disposed path finder can be safely reinitialized. Any children owned
     * by this path finder will be released. They will be deleted if no other
     * object owns them.
     */
    virtual void dispose();

    /**
     * Intializes a path finder for a given agent and world.
     * 
     * The grid representation is generated for the underlying
     * world for the specified number of rows and columns for
     * the grid.
     * 
     * @param world the world to run pathfinding on
     * @param agent the path finding agent
     * @param h the heurstic function
     * @param glength number of columns in path finding grid
     * @param gwidth number of rows in path finding grid
     * @param smoothing whether to perform path smoothing
     * 
     * @return true if the node is initialized properly, false otherwise.
     */
    bool init(const std::shared_pointer<ObstacleWorld>& world, 
              const std::shared_pointer<Obstacle>& agent,
              const HeuristicFunction& h, int gwidth,
              int glength, bool smoothing);

#pragma mark Static Constructors
    /**
     * Allocated a path finder for a given agent and world.
     * 
     * The grid representation is generated for the underlying
     * world for the specified number of rows and columns for
     * the grid.
     * 
     * @param world the world to run pathfinding on
     * @param agent the path finding agent
     * @param h the heurstic function
     * @param glength number of columns in path finding grid
     * @param gwidth number of rows in path finding grid
     * @param smoothing whether to perform path smoothing
     * 
     * @return a newly allocated path finder.
     */
    static std::shared_pointer<PathFinder> alloc(
              const std::shared_pointer<ObstacleWorld> world, 
              const std::shared_pointer<Obstacle> agent,
              const HeuristicFunction& h, int gwidth,
              int glength, bool smoothing);

#pragma mark Attributes
    /**
     * Returns the state of this path finder
     * 
     * @return the state of this pathfinder.
     */
    PathFinder::State getState() const { return _state; };

    /**
     * Returns the computed path for this path finder.
     * 
     * If the path has not yet been computed, the computed path
     * will only contain the path finding agent's starting location.
     * 
     * @return the computed path for this path finder.
     */
    const vector<Vec2>& getPath() const { return _path; }

    /**
     * Return the agent for this path finder.
     * 
     * @return the agent for this path finder.
     */
    const std::shared_pointer<Obstacle>& getAgent() const { return _agent; }

    /**
     * Return the world for this path finder.
     * 
     * @return the world for this path finder.
     */
    const std::shared_pointer<ObstacleWorld>& getWorld() const { return _world; }

    /**
     * Return the heuristic function for this path finder.
     * 
     * @return the heuristic function for this path finder.
     */
    const HeuristicFunction& getHeuristicFunction() const { return _h; }

    /**
     * Return whether this path finder will perform path smoothing.
     * 
     * @return true if this path finder will perform path smoothing, else false
     */
    bool getSmoothing() const { return _smoothing; }

#pragma mark Path Finding

    /**
     * Computes the optimal path to the target location.
     * 
     * @param target the target location for the pathfinding agent.
     * 
     * @return the computed path to the target location.  
     */
    virtual const vector<Vec2>& computePath(const Vec2& target);

    /**
     * Returns the section of the optimal path not yet traversed.
     * 
     * @return the section of the optimal path not yet traversed.
     */
    const std::vector<Vec2> getUntraversedPath() const;

    /**
     * Computes the optimal path to the target location and moves the
     * pathfinding agent to that target location. 
     * 
     * This method will run concurrently with the update and will not
     * return until either the target location is reached or there is
     * no possible path to the target location. Changes to the optimal
     * path will affect the agent's movement.
     * 
     * @param target the target location for the pathfinding agent
     * @param velocity the velocity of the pathfinding agent
     * 
     * @return whether agent has successfully reached the target location.
     */
    virtual bool move(const Vec2& target, const Vec2& velocity);

    /**
     * Stops the pathfinding agent's movement.
     * 
     * To resume pathfinding after this method has been called, the optimal
     * path must be recomputed.
     */
    virtual void stop();

    /**
     * Runs an update function, meant to be used on each tick, for the
	 * path finder.
     * 
     * The optimal path may be updated by this method, based on the
     * path finding algorithm and the heuristic function.
     * 
     * @return the current state of the path finder. 
     */
    virtual State update();
};

};

#endif