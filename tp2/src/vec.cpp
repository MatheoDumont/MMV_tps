#include "vec.h"
#include <cmath>

bool vec3::operator==(const vec3 &v) const
{
    return (x == v.x) && (y == v.y) && (z == v.z);
}

bool vec2::operator==(const vec2 &v) const
{
    return (x == v.x) && (y == v.y);
}

std::ostream &operator<<(std::ostream &o, const vec3 &v)
{
    o << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return o;
}

std::ostream &operator<<(std::ostream &o, const vec2 &v)
{
    o << "(" << v.x << ", " << v.y << ")";
    return o;
}

vec3 operator-(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

vec2 operator-(const vec2 &v1, const vec2 &v2)
{
    return vec2(v1.x - v2.x, v1.y - v2.y);
}

vec3 operator*(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

vec3 operator*(const vec3 &v, float k)
{
    return vec3(v.x * k, v.y * k, v.z * k);
}

vec3 operator*(float k, const vec3 &v)
{
    return v * k;
}

vec2 operator*(const vec2 &v1, const vec2 &v2)
{
    return vec2(v1.x * v2.x, v1.y * v2.y);
}

vec2 operator*(const vec2 &v, float k)
{
    return vec2(v.x * k, v.y * k);
}

vec2 operator*(float k, const vec2 &v)
{
    return v * k;
}

vec3 operator/(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

vec3 operator/(const vec3 &v, float k)
{
    return vec3(v.x / k, v.y / k, v.z / k);
}

vec2 operator/(const vec2 &v1, const vec2 &v2)
{
    return vec2(v1.x / v2.x, v1.y / v2.y);
}

vec2 operator/(const vec2 &v, float k)
{
    return vec2(v.x / k, v.y / k);
}

vec3 operator+(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

vec2 operator+(const vec2 &v1, const vec2 &v2)
{
    return vec2(v1.x + v2.x, v1.y + v2.y);
}

float length(const vec3 &v)
{
    return sqrt((v.x * v.x) +
                (v.y * v.y) +
                (v.z * v.z));
}

float length(const vec2 &v)
{
    return sqrt((v.x * v.x) +
                (v.y * v.y));
}

vec3 normalize(const vec3 &v)
{
    return v / length(v);
}

vec2 normalize(const vec2 &v)
{
    return v / length(v);
}

float dot(const vec3 &v1, const vec3 &v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

float dot(const vec2 &v1, const vec2 &v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y);
}

vec3 cross(const vec3 &v1, const vec3 &v2)
{
    vec3 v;
    v.x = (v1.y * v2.z) - (v1.z * v2.y);
    v.y = (v1.z * v2.x) - (v1.x * v2.z);
    v.z = (v1.x * v2.y) - (v1.y * v2.x);
    return v;
}

