#include "scalar_field.h"

SF::SF() : Grid(), field() {}
SF::SF(const Grid &g) : Grid(g), field(nx * ny, 0.0) {}
SF::SF(const SF &sf) : Grid(Box2D(sf.a, sf.b), sf.nx, sf.ny), field(sf.field) {}

double SF::at(int i, int j) const
{
    return field[index(i, j)];
}

double &SF::at(int i, int j)
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
    else if (j == ny - 1)
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

double SF::normalization(double x, double in_min, double in_max, double out_min, double out_max) const
{
    x = (x - in_min) / (in_max - in_min);
    return x * (out_max - out_min) + out_min;
}

double SF::normalization(double x, double out_min, double out_max) const
{
    return normalization(x, 0.0, 1.0, out_min, out_max);
}

double SF::clamp(double min, double max, double x) const
{
    if (x >= max)
        return max;
    else if (x <= min)
        return min;

    return x;
}

vec2 SF::clamp(const vec2 &) const
{
    return vec2(); // TODO
}

// const double mask_smooth[9] = {0.1111, 0.1111, 0.1111,
//                                0.1111, 0.1111, 0.1111,
//                                0.1111, 0.1111, 0.1111};

const double mask_smooth[3][3] = {{0.1111111, 0.1111111, 0.1111111},
                                  {0.1111111, 0.1111111, 0.1111111},
                                  {0.1111111, 0.1111111, 0.1111111}};
// const double mask_blur[9] = {0.05854983, 0.09653235, 0.05854983,
//                              0.09653235, 0.15915494, 0.09653235,
//                              0.05854983, 0.09653235, 0.05854983};

const double mask_blur[3][3] = {{0.05854983, 0.09653235, 0.05854983},
                                {0.09653235, 0.15915494, 0.09653235},
                                {0.05854983, 0.09653235, 0.05854983}};

void applyMask(const SF &sf, SF &sf_filtered, int i, int j, const double mask[3][3])
{
    for (int ii = -1; ii < 2; ++ii)
        for (int jj = -1; jj < 2; ++jj)
            sf_filtered.at(i + ii, j + jj) += mask[ii + 1][jj + 1] * sf.at(i + ii, j + jj);
}

void applyFilter(const SF &sf, SF &sf_filtered, const double mask[3][3])
{
    for (int i = 1; i < sf_filtered.getNX() - 1; ++i)
        for (int j = 1; j < sf_filtered.getNY() - 1; ++j)
            applyMask(sf, sf_filtered, i, j, mask);
}

SF SF::filter(const SF &sf, FilterType t)
{
    SF sf_filtered(Grid(Box2D(sf.a, sf.b), sf.nx, sf.ny));

    switch (t)
    {
    case FilterType::Smooth:
        applyFilter(sf, sf_filtered, mask_smooth);
        break;
    case FilterType::Blur:
        applyFilter(sf, sf_filtered, mask_blur);
        break;
    default:
        break;
    }
    return sf_filtered;
}
