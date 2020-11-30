#ifndef GRID_H
#define GRID_H
#include "box2d.h"

class Grid : Box2D
{
private:
    int nx, ny;

public:
    Grid(const Box2d &b, int nx, int ny);
    ~Grid();

    int index(int i, int j) const;
    bool inside(int, int) const;
    bool border(int, int) const;
};

#endif