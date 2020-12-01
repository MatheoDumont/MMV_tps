#ifndef BOX_2_H
#define BOX_2_H

#include "vec.h"

class Box2D
{
protected:
    vec2 a, b;
    vec2 diagonal;

public:
    Box2D();
    Box2D(const vec2 &_a, const vec2 &_b);
    Box2D(const Box2D &box);
    Box2D(const vec2 &center, double radius);

    bool inside(const vec2 &v) const;
    bool intersect(const Box2D &b) const;
};

#endif
