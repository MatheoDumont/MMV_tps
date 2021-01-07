#ifndef GRID_H
#define GRID_H

#include "box2d.h"
#include "vec.h"

class Grid : public Box2D
{
protected:
    int nx, ny;
    // vec2 diagonal; pas besoins, on stock deja la diagonal dans Box2D
    vec2 celldiagonal;
    vec2 inversecelldiagonal;

public:
    Grid();
    Grid(const Box2D &box, int _nx, int _ny);

    int index(int i, int j) const;
    std::pair<int, int> inverseIndex(int x) const;
    bool inside(int i, int j) const;
    bool border(int i, int j) const;
    vec2 vertex(int i, int j) const;
    inline int getNX() const { return nx; }
    inline int getNY() const { return ny; }
};

#endif
