#include "grid.h"

Grid::Grid() : Box2D() {}
Grid::Grid(const Box2D &box, int _nx, int _ny) : Box2D(box), nx(_nx), ny(_ny)
{
    celldiagonal = vec2(diagonal.x / double(nx - 1),
                        diagonal.y / double(ny - 1));

    inversecelldiagonal = vec2(1.0 / celldiagonal.x,
                               1.0 / celldiagonal.y);
}

int Grid::index(int i, int j) const
{
    return j + i * nx;
}

std::pair<int, int> Grid::inverseIndex(int x) const
{
    return {x % nx, x / nx};
}

bool Grid::inside(int i, int j) const
{
    return (i >= 0 && i < nx && j >= 0 && j < ny);
}

bool Grid::border(int i, int j) const
{
    return (i == 0 || i == nx - 1 || j == 0 || j == ny - 1);
}

vec2 Grid::vertex(int i, int j) const
{
    double u = double(i) / (nx - 1);
    double v = double(j) / (ny - 1);

    return vec2((1 - u) * a.x + u * b.x,
                (1 - v) * a.y + v * b.y);
}
