#ifndef BOX_2_H
#define BOX_2_H

#include "vec.h"

class Box2D
{
protected:
    vec2 origin;
    float length;

public:
    Box2D(vec2 origin, float length);
    // ~Box2D();

    bool inside(const vec2 &v) const;
    bool intersect(const Box2D &b) const;
};

#endif
