#include "scalar_field.h"

SF::SF(const Grid &g) : Grid(g)
{
    field.resize(nx * ny, 0.0);
}

double SF::at(int i, int j) const
{
    return field[index(i, j)];
}

double& SF::at(int i, int j)
{
    return field[index(i, j)];
}

vec2 SF::gradient(int i, int j) const
{
    vec2 grad;

    // epsilon = 1 dans ce cas la
    if (i == 0)
        grad.x = (at(i + 1, j) - at(i, j)) * inversecelldiagonal.x;
    else if (i == nx - 1)
        grad.x = (at(i, j) - at(i - 1, j)) * inversecelldiagonal.x;
    else // cas general
        grad.x = (at(i + 1, j) - at(i - 1, j)) * 0.5 * inversecelldiagonal.x;

    if (j == 0)
        grad.y = (at(i, j + 1) - at(i, j)) * inversecelldiagonal.y;
    else if (i == ny - 1)
        grad.y = (at(i, j) - at(i, j - 1)) * inversecelldiagonal.y;
    else // cas general
        grad.y = (at(i, j + 1) - at(i, j - 1)) * 0.5 * inversecelldiagonal.y;

    return grad;
}

double SF::laplacian(int i, int j) const
{
    double laplacian = 0.0;

    if (i == 0)
        laplacian += (at(i, j) - 2.0 * at(i + 1, j) + at(i + 2, j)) * (inversecelldiagonal.x * inversecelldiagonal.x);
    else if (i == nx - 1)
        laplacian += (at(i, j) - 2.0 * at(i - 1, j) + at(i - 2, j)) * (inversecelldiagonal.x * inversecelldiagonal.x);
    else
        laplacian += (at(i + 1, j) - 2.0 * at(i, j) + at(i - 1, j)) * (inversecelldiagonal.x * inversecelldiagonal.x);

    if (j == 0)
        laplacian += (at(i, j) - 2.0 * at(i, j + 1) + at(i, j + 2)) * (inversecelldiagonal.y * inversecelldiagonal.y);
    else if (j == ny - 1)
        laplacian += (at(i, j) - 2.0 * at(i, j - 1) + at(i, j - 2)) * (inversecelldiagonal.y * inversecelldiagonal.y);
    else
        laplacian += (at(i, j + 1) - 2.0 * at(i, j) + at(i, j - 1)) * (inversecelldiagonal.y * inversecelldiagonal.y);
	
    return laplacian;
}

double SF::normalization(double x, double boundmin, double boundmax) const
{
    return x * (boundmax - boundmin) + boundmin;
}

vec2 SF::clamp(const vec2 &) const
{
    return vec2(); // TODO
}
