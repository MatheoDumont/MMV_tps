#include "box2d.h"

Box2D::Box2D() {}
Box2D::Box2D(const vec2 &_a, const vec2 &_b) : a(_a), b(_b), diagonal(_b - _a) {}
Box2D::Box2D(const Box2D &box) : a(box.a), b(box.b), diagonal(box.diagonal) {}

Box2D::Box2D(const vec2 &center, double radius)
{
    a = center - vec2(radius);
    b = center + vec2(radius);
    diagonal = b - a;
}

bool Box2D::inside(const vec2 &v) const
{
    if (v.x < a.x || v.x > b.x ||
        v.y < a.y || v.y > b.y)
        return false;
    else
        return true;
}

bool Box2D::intersect(const Box2D &box) const
{
    if (a.x >= box.b.x || a.y >= box.b.y ||
        b.x <= box.a.x || b.y <= box.a.y)
        return false;
    else
        return true;
}
