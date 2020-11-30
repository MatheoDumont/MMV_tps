#ifndef SCALAR_FIELD_H
#define SCALAR_FIELD_H

#include <vector>
#include "grid.h"
#include "vec.h"

class SF2 : public Grid
{
protected:
	std::vector<double> field;

public:
	SF2(const Grid& g);

	const double at(int i, int j) const;
	double& at(int i, int j);

    vec2 gradient(int i, int j) const;
    vec2 laplacien(int i, int j) const;
};

#endif
