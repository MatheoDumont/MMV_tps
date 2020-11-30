#include "grid.h"

int Grid::index(int i, int j) const
{
    return i + j * nx;
}

bool Grid::inside(int, int) const
{
}

bool Grid::border(int, int) const
{
}