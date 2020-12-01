#include "box2d.h"

Box2D::Box2D(vec2 _origin, float _length) : origin(_origin), length(_length) {}

bool Box2D::inside(const vec2 &v) const
{
    if (v.x < origin.x || v.x > (origin.x + length) ||
        v.y < origin.y || v.y > (origin.y + length))
        return false;
    else
        return true;
}

bool Box2D::intersect(const Box2D &b) const
{
    if ((origin.x >= b.origin.x + b.length) ||
        (origin.y >= b.origin.y + b.length) ||
        (origin.x + length <= b.origin.x) ||
        (origin.y + length <= b.origin.y))
        return false;
    else
        return true;
}
