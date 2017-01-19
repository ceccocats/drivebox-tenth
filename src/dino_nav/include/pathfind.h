#ifndef PATHFIND_H
#define PATHFIND_H

#include "grid.h"


/** Struttura contenente i dati dei nodi utilizzati nel ::pathfinding */
struct node_t {
    point_t pos;
    dir_e dir;
    float cost;

    //this are for node identification
    node_t *parent;
};

const int MAX_ITER = 5000;

struct path_t {

    float_point_t data[MAX_ITER];
    int size;

    int start;
};

path_t pathfinding(grid_t &grid, view_t &view, point_t &s, point_t &e);

#endif //PATHFIND_H