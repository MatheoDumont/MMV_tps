#include "scalar_field.h"

SF2::SF2(const Grid &g) : Grid(g)
{
	field.resize(nx * ny, 0.0);
}

const double SF2::at(int i, int j) const
{
	return field[index(i, j)];
}

double &SF2::at(int i, int j)
{
	return field[index(i, j)];
}

/*
 * Dérivée première
 * Propriété simple on peut l'approximer avec :
 * df/dx, df/dy ~ ((f(x+e, y) - f(x-e,y)) / 2e, ...pareil pour y )
 */
vec2 SF2::gradient(int i, int j) const
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

/*
 * Dérivée seconde
 * Propriété simple on peut l'approximer avec :
 * d2f / dx2 ~ (f(x + epsilon) - 2f(x) + f(x + epsilon)) / (epsilon * epsilon)
 * https://fr.wikipedia.org/wiki/Laplacien_discret
 */
vec2 SF2::laplacien(int i, int j) const
{

	double laplacian = 0.0;

	if (i == 0)
		laplacian += (at(i, j) - 2.0 * at(i + 1, j) + at(i + 2, j)) / (celldiagonal.x * celldiagonal.x);
	else if (i == nx - 1)
		laplacian += (at(i, j) - 2.0 * at(i - 1, j) + at(i - 2, j)) / (celldiagonal.x * celldiagonal.x);
	else
		laplacian += (at(i + 1, j) - 2.0 * at(i, j) + at(i - 1, j)) / (celldiagonal.x * celldiagonal.x);

	if (j == 0)
		laplacian += (at(i, j) - 2.0 * at(i, j + 1) + at(i, j + 2)) / (celldiagonal.y * celldiagonal.y);
	else if (j == ny - 1)
		laplacian += (at(i, j) - 2.0 * at(i, j - 1) + at(i, j - 2)) / (celldiagonal.y * celldiagonal.y);
	else
		laplacian += (at(i, j + 1) - 2.0 * at(i, j) + at(i, j - 1)) / (celldiagonal.y * celldiagonal.y);
}
