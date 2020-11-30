#include "grid.h"

Grid::Grid(const Box2D &b, int _nx, int _ny) : Box2D(b), nx(_nx), ny(_ny)
{
    celldiagonal = vec2(length / nx, length / ny);
    inversecelldiagonal = vec2(1.0 / celldiagonal.x, 1.0 / celldiagonal.y);
}

int Grid::index(int i, int j) const
{
    return i + j * nx;
}

bool Grid::inside(int i, int j) const
{
    return ((i >= 0) && (i < nx) && (j >= 0) && (j < ny));
}

bool Grid::border(int i, int j) const
{
    return ((i == 0) || (i == nx - 1)) || ((j == 0) || (j == ny - 1));
}

vec2 Grid::vertex(int i, int j) const
{
    double u = double(i) / (nx - 1);
    double v = double(j) / (ny - 1);

    vec2 p = origin + length;

    return vec2((1 - u) * origin.x + u * p.x, (1 - v) * origin.y + v * p.y);
}
