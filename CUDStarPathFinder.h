#ifndef __CU_D_Star_Path_Finder_H__
#define __CU_D_Star_Path_Finder_H__

#include "CUPathFinder.h"

namespace cugl {

/**
 * This class provides a path finder that uses D* search for a 
 * given agent and world.
 * 
 * A path finder will compute the optimal path for an agent to
 * a target location using an algorithm specific to each type
 * of path finder and move the agent along the computed path.
 * A path finder must use update to run on each tick, to move 
 * the agent along that path. The optimal path will be recomputed on
 * obstruction using D* search.
 */
class DStarPathFinder : public PathFinder {
public:
#pragma mark Constructor
    /**
     * Creates a new D* search path finder.
     *
     * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
     * the heap, use one of the static constructors instead.
     */
    DStarPathFinder();

    /**
     * Deletes this D* path finder, disposing all resources.
     */
    virtual ~DStarPathFinder();

#pragma mark Static Constructors
    /**
     * Allocated a D* path finder for a given agent and world.
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
    static std::shared_pointer<DStarPathFinder> alloc(
              const std::shared_pointer<ObstacleWorld> world, 
              const std::shared_pointer<Obstacle> agent,
              const HeuristicFunction& h, int gwidth,
              int glength, bool smoothing);


#pragma mark Path Finding
    /**
     * Computes the optimal path to the target location using D* search.
     * 
     * @param target the target location for the pathfinding agent.
     * 
     * @return the computed path to the target location.  
     */
    virtual const vector<Vec2>& computePath(const Vec2& target) override;

    /**
     * Computes the optimal path to the target location using D* search
     * and moves the pathfinding agent to that target location. 
     * 
     * This method will run concurrently with the update and will not
     * return until either the target location is reached or there is
     * no possible path to the target location.
     * 
     * @param target the target location for the pathfinding agent
     * @param velocity the velocity of the pathfinding agent
     * 
     * @return whether agent has successfully reached the target location.
     */
    virtual bool move(const Vec2& target, const& Vec2& velocity) override;

    /**
     * Stops the pathfinding agent's movement.
     * 
     * To resume pathfinding after this method has been called, the optimal
     * path must be recomputed.
     */
    virtual void stop() override;

    /**
     * Runs an update function, meant to be used on each tick, for the
	 * path finder.
     * 
     * If the previously computed optimal path is obstructed, the update
     * method will recompute the optimal path using D* search.
     * 
     * @return the current state of the path finder. 
     */
    virtual State update() override;

};

}

#endif