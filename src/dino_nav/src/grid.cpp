//
// Created by cecco on 07/12/16.
//
#include <math.h>
#include <stdlib.h>

#include "ros/ros.h"
#include "grid.h"
#include "dinonav.h"


void init_grid(grid_t &grid, int size) {

    grid.size = size;

    //init grid
    for(int i=0; i<size*size; i++)
        grid.data[i] = EMPTY;
    grid.gates_n = 0;
    grid.points_n = 0;
}


/**
    geterate a line in a matrix from point 1 to 2
*/
int grid_line(grid_t &grid, int x1, int y1, int x2, int y2, int value) {
    //gate search
    int startGx = -1, startGy = -1;
    int endGx = -1,   endGy = -1;

    int dx = x1 - x2;
    int dy = y1 - y2;
    int steps;

    if (abs(dx) > abs(dy))
        steps = abs(dx);
    else
        steps = abs(dy);

    float x_inc = -(float)dx / (float) steps;
    float y_inc = -(float)dy / (float) steps;

    if(steps > grid.size)
        return 0;

    float x = x1, y = y1;
    for(int v=0; v < steps; v++)
    {
        x = x + x_inc;
        y = y + y_inc;
        int xx = x, yy = y;
        if((value == 0 &&  getgrid(grid, xx, yy) == GATE) || getgrid(grid, xx, yy) < value) {
            setgrid(grid, xx, yy, value);
            if(startGx == -1) {
                startGx = xx; startGy = yy;
            } else {
                endGx = xx; endGy = yy;
            }
        }
    }

    if(grid.gates_n >= GRID_MAX_GATES) {
        ROS_WARN("Reached max gates");
        return 0;
    }

    if(startGx != -1 && endGx != -1) {
        //startGx and endGx are effective gate bounds 
        gate_t *g = &grid.gates[grid.gates_n];
        g->s.x = x1;
        g->s.y = y1;
        g->e.x = x2;
        g->e.y = y2;
        g->dim = steps;
        grid.gates_n++;
        //printf("start: %d %d, end: %d %d\n", startGx, startGy, endGx, endGy);
    }
    return steps;
}

/**
    geterate a line in a matrix from point 1 to 2
*/
bool grid_line_control(grid_t &grid, int x1, int y1, int x2, int y2) {

    int dx = x1 - x2;
    int dy = y1 - y2;
    int steps;

    if (abs(dx) > abs(dy))
        steps = abs(dx);
    else
        steps = abs(dy);

    float x_inc = -(float)dx / (float) steps;
    float y_inc = -(float)dy / (float) steps;

    float x = x1, y = y1;
    for(int v=0; v < steps; v++)
    {
        x = x + x_inc;
        y = y + y_inc;
        int xx = x, yy = y;
        int val = getgrid(grid, xx, yy);
        int val1 = getgrid(grid, xx + 1, yy);
        int val2 = getgrid(grid, xx - 1, yy);
        if(val != EMPTY || val1 != EMPTY || val2 != EMPTY)
            return false;

        //setgrid(grid, xx +1, yy, 10);
        //setgrid(grid, xx -1, yy, 10);
        //setgrid(grid, xx, yy, 10);
    }
    return true;
}


/**
    set a grid to value in given position
*/
bool setgrid(grid_t &grid, int x, int y, int value) {
    int pos = y*grid.size + x;

    if(x<0 || x >= grid.size || y<0 || y >= grid.size)
        return false;
    grid.data[pos] = value;
    return true;
}

int getgrid(grid_t &grid, int x, int y) {
    int pos = y*grid.size + x;

    if(x<0 || x >= grid.size || y<0 || y >= grid.size)
        return -1;
    return grid.data[pos];
}

/**
    inflate a point with the given spread
          X X X
    X ->  X X X   example with n = 1
          X X X
*/
void inflate(grid_t &grid, int x, int y, int val, int n) {
    if(n == 0)
        return;

    if(getgrid(grid, x, y) != WALL)
        setgrid(grid, x, y, val);

    inflate(grid, x-1, y-1, val, n -1);
    inflate(grid, x-1, y, val, n -1);
    inflate(grid, x-1, y+1, val, n -1);
    inflate(grid, x+1, y-1, val, n -1);
    inflate(grid, x+1, y, val, n -1);
    inflate(grid, x+1, y+1, val, n -1);
    inflate(grid, x, y-1, val, n -1);
    inflate(grid, x, y+1, val, n -1);
}


int choosegate(grid_t &grid, int px, int py) {

    int max_dim = 0;
    int idx = 0;
    for(int i=0; i<grid.gates_n; i++) {
        if(grid.gates[i].dim > max_dim)  {
            max_dim = grid.gates[i].dim;
            idx = i; 
        }
    }
    
    return idx;
}

float_point_t grid2view(int x, int y, view_t &view) {
    float_point_t p;
    p.x = view.x + x*view.cell_l + view.cell_l/2;
    p.y = view.y + y*view.cell_l + view.cell_l/2;
    return p;
}


point_t view2grid(float x, float y, view_t &view) {
    point_t p;
    p.x = (x - view.x)/view.cell_l;
    p.y = (y - view.y)/view.cell_l;

    return p;
}