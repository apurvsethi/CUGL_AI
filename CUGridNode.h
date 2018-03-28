#ifndef __CU_GRID_NODE_H__
#define __CU_GRID_NODE_H__

#include <cugl/math/CURect.h>
#include <cugl/2d/physics/CUObstacle.h>

namespace cugl {

/**
 * This class describes the individual nodes of the pathfinding grid.
 * 
 * Each grid node corresponds to a square in the world. A grid node is a 
 * container for the computed cost of the path to this grid node and 
 * whether this node is obstructed. These values should be updated by a
 * path finding agent.
 */
class GridNode {
#pragma mark Values
private:
    /** The bounds of this grid node */
    Rect _bounds;

    /** Index of this node in the grid */
    std::pair<int, int> _index;
    
    /** The cost of the path to this node in a Pathfinding search */
    float _heuristic;
    
    /** The priority of this node, to be set in a D* search */
    float _priority;

    /** The clearance value of this node. */
    Uint32 _clearance;

    /** Whether this node is obstructed by an obstacle */
    bool _obstructed;

    /** The neighbors of this node */
    std::set<std::shared_pointer<GridNode>> _neighbors;
public:
#pragma mark Constructors
    /**
     * Creates a new grid node.
     *
     * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
     * the heap, use one of the static constructors instead.
     */
    GridNode();

    /**
     * Deletes this grid node, disposing all resources
     */
    virtual ~GridNode() { dispose(); }

    /**
     * Disposes all of the resources used by this grid node.
     *
     * A disposed GridNode can be safely reinitialized. Any children owned by this
     * node will be released.  They will be deleted if no other object owns them.
     *
     * It is unsafe to call this on a GridNode that is still currently inside of
     * a grid.
     */
    void dispose();

    /**
     * Initializes a new grid node.
     *
     * @param dimensions the size of the node
     * @param x_index the x-coordinate of the node on the grid
     * @param y_index the y-coordinate of the node on the grid
     * 
     * @return true if the node is initialized properly, false otherwise.
     */
    bool init(const Size& size, int x_index, int y_index);

#pragma mark Static Constructors

    /**
     * Returns a newly allocated grid node.
     *
     * @param dimensions the size of the node
     * @param x_index the x-coordinate of the node on the grid
     * @param y_index the y-coordinate of the node on the grid
     *
     * @return a newly allocated grid node.
     */
    static std::shared_ptr<GridNode> alloc(const Size& size, int x_index, int y_index);

#pragma mark Attributes

    /**
     * Returns the bounds of this grid node
     * 
     * @return the bounds of this grid node.
     */
    const Rect& getBounds() { return _bounds; }

    /**
     * Returns the index of this grid node
     * 
     * @return the index of this grid node. 
     */
    const std::pair<int, int>&  getIndex() { return _index; }

    /**
     * Returns the heuristic of this grid node
     * 
     * @return the heuristic of this grid node.
     */
    float getHeuristic() { return _heuristic; }

    /**
     * Sets the heuristic for this grid node.
     * 
     * @param h the heuristic to set for this node 
     */
    void setHeuristic(float h) { _heuristic = h; }
    
    /**
     * Returns the priority of this grid node
     * 
     * @return the priority of this grid node.
     */
    float getPriority() { return _priority; }

    /**
     * Sets the priority for this grid node.
     * 
     * @param k the priority to set for this node 
     */
    void setPriority(float k) { _priority = k; }

    /**
     * Returns the clearance of this grid node
     * 
     * @return the clearance of this grid node.
     */
    float getClearance() { return _clearance; }

    /**
     * Sets the clearance for this grid node.
     * 
     * @param clearance the clearance to set for this node 
     */
    void setClearance(float clearance) { _clearance = clearance; }

#pragma mark Neighbors

    /**
     *  Returns the neighbors of this grid node.
     * 
     * @return the neighbors of this grid node.
     */
    const std::set<std::shared_pointer<GridNode>>& getNeighbors() const { return _neighbors; }

    /**
     * Adds a neighbor to this grid node if not it is not yet in the 
     * neighboring nodes.
     * 
     * @param neighbor the grid node to add as neighbor
     */
    void addNeighbor(const std::shared_pointer<GridNode>& neighbor) { _neighbors.insert(neighbor); }

#pragma mark Obstructed

    /**
     * Returns whether this grid node is obstructed by some obstacle.
     * 
     * @return whether this grid node is obstructed by some obstacle.
     */
    bool isObstructed() { return _obstructed; }

    /**
     *  Sets the obstacle to not obstructed. 
     */
    void setNotObstructed() { _obstructed = false; }

    /**
     * Checks whether an obstacle is obstructing this grid node. If
     * it is, marks this grid node as obstructed.
     * 
     * An obstacle is obstructing this grid node if the obstacle's fixture is
     * overlapping this grid node's bounds.
     * 
     * @param obstacle the obstacle to check for obstruction
     * 
     * @return whether this obstacle is overlapping the grid node.
     */
    bool checkObstruction(const std::shared_pointer<Obstacle>& obstacle);
};

}

#endif