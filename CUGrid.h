#ifndef __CU_Grid_H__
#define __CU_Grid_H__

#include "CUGridNode.h"
#include <cugl/2d/math/Vec2.h>
#include <cugl/2d/physics/CUObstacleWorld.h>

namespace cugl {

/**
 * A grid representation the world for path finding. 
 */
class Grid {
public:

#pragma mark -
#pragma mark Constructors

    /**
     * Creates a new grid.
     *
     * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
     * the heap, use one of the static constructors instead.
     */
    Grid();

    /**
     * Deletes this grid, disposing all resources
     */
    virtual ~Grid() { dispose(); }

    /**
     * Disposes all of the resources used by this grid.
     *
     * A disposed Grid can be safely reinitialized. Any children owned by this
     * grid will be released.  They will be deleted if no other object owns them.
     *
     * It is unsafe to call this on a Grid that is still currently being used
     * by a path finder.
     */
    void dispose();

    /**
     * Initializes a new grid.
     * 
     * @param bounds the bounds of the world
     * @param rows the number of rows of this grid
     * @param cols the number of columns of this grid
     * 
     * @return true if the node is initialized properly, false otherwise.
     */
    bool init(const Rect& bounds, Uint32 rows, Uint32 cols);

#pragma mark -
#pragma mark Static Constructors

    /**
     * Returns a newly allocated grid.
     * 
     * @param bounds the bounds of the world
     * @param rows the number of rows of this grid
     * @param cols the number of columns of this grid
     * 
     * @return a newly allocated grid
     */
    static std::shared_pointer<Grid> alloc(const Rect& bounds, Uint32 rows, Uint32 cols);

#pragma mark -
#pragma mark Attributes

    /**
     * Returns the number of rows of this grid.
     * 
     * @return the number of rows of this grid.
     */
    Uint32 getRows() { return _gwidth; }

    /**
     * Returns the number of columns of this grid.
     * 
     * @return the number of columns of this grid.
     */
    Uint32 getColumns() { return _gwidth; }

    /**
     * Return the nodes of this grid.
     * 
     * @return the nodes of this grid.
     */
    const std::vector<std::shared_pointer<GridNodes> getNodes() { return nodes; }

    /**
     * Return the node at a specified row and column index.
     * 
     * @param row the row index of the node to retrieve
     * @param col the column index of the node to retrieve
     * 
     * @return the node at the specified row and column index
     */
    const std::shared_pointer<GridNode> getNodeAt(int row, int col);

    /**
     * Return the node containing the specified location. 
     * 
     * @param location location to get the node at in world coordinates
     * 
     * @return the node at the specified location.
     */
    const std::shared_pointer<GridNode> getNodeAtLocation(Vec2 location);


private:
    /** The nodes of this grid. */
    std::vector<std::shared_pointer<GridNode> _nodes;

    /** The number of rows of this grid. */
    Uint32 _nrows;

    /** The number of columns of this grid. */
    Uint32 _ncols;
};

}

#endif