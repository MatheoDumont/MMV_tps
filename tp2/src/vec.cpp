#include "vec.h"
#include <cmath>

vec2::vec2() : x(0.0), y(0.0) {}
vec2::vec2(double a) : x(a), y(a) {}
vec2::vec2(double _x, double _y) : x(_x), y(_y) {}
vec2::vec2(const vec2& v) : x(v.x), y(v.y) {}

double vec2::length() const
{
    return sqrt((x * x) +
                (y * y));
}

vec2 vec2::normalize() const
{
    return vec2( (*this) / length() );
}

vec2& vec2::operator=(const vec2 &rhs)
{
    if (this != &rhs)
    {
        x = rhs.x;
        y = rhs.y;
    }

    return *this;
}

bool vec2::operator==(const vec2 &rhs) const
{
    return (x == rhs.x) && (y == rhs.y);
}

vec2 operator+(double k, const vec2 &v)
{
    return vec2(v.x + k, v.y + k);
}

vec2 operator+(const vec2 &v, double k)
{
    return vec2(v.x + k, v.y + k);    
}

vec2 operator+(const vec2 &v1, const vec2 &v2)
{
    return vec2(v1.x + v2.x, v1.y + v2.y);
}

vec2 operator-(const vec2 &v)
{
    return vec2(-v.x, -v.y);
}

vec2 operator-(const vec2 &v1, const vec2 &v2)
{
    return vec2(v1.x - v2.x, v1.y - v2.y);
}

vec2 operator*(double k, const vec2 &v)
{
    return vec2(v.x * k, v.y * k);
}

vec2 operator*(const vec2 &v, double k)
{
    return vec2(v.x * k, v.y * k);
}

vec2 operator*(const vec2 &v1, const vec2 &v2)
{
    return vec2(v1.x * v2.x, v1.y * v2.y);
}

vec2 operator/(const vec2 &v, double k)
{
    return vec2(v.x / k, v.y / k);
}

double dot(const vec2 &v1, const vec2 &v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y);
}

std::ostream &operator<<(std::ostream &o, const vec2 &v)
{
    o << "(" << v.x << ", " << v.y << ")";
    return o;
}

/*************************************************/

vec3::vec3() : x(0.0), y(0.0), z(0.0) {}
vec3::vec3(double a) : x(a), y(a), z(a) {}
vec3::vec3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
vec3::vec3(const vec3& v) : x(v.x), y(v.y), z(v.z) {}
vec3::vec3(const vec2& v, double _z) : x(v.x), y(v.y), z(_z) {}

double vec3::length() const
{
    return sqrt((x * x) +
                (y * y) +
                (z * z));
}

vec3 vec3::normalize() const
{
    return vec3( (*this) / length() );
}

vec3& vec3::operator=(const vec3 &rhs)
{
    if (this != &rhs)
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
    }

    return *this;
}

bool vec3::operator==(const vec3 &rhs) const
{
    return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}

vec3 operator+(double k, const vec3 &v)
{
    return vec3(v.x + k, v.y + k, v.z + k);
}

vec3 operator+(const vec3 &v, double k)
{
    return vec3(v.x + k, v.y + k, v.z + k);    
}

vec3 operator+(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

vec3 operator-(const vec3 &v)
{
    return vec3(-v.x, -v.y, -v.z);
}

vec3 operator-(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

vec3 operator*(double k, const vec3 &v)
{
    return vec3(v.x * k, v.y * k, v.z * k);
}

vec3 operator*(const vec3 &v, double k)
{
    return vec3(v.x * k, v.y * k, v.z * k);
}

vec3 operator*(const vec3 &v1, const vec3 &v2)
{
    return vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

vec3 operator/(const vec3 &v, double k)
{
    return vec3(v.x / k, v.y / k, v.z / k);
}

double dot(const vec3 &v1, const vec3 &v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

vec3 cross(const vec3 &v1, const vec3 &v2)
{
    vec3 v;
    v.x = (v1.y * v2.z) - (v1.z * v2.y);
    v.y = (v1.z * v2.x) - (v1.x * v2.z);
    v.z = (v1.x * v2.y) - (v1.y * v2.x);
    return v;
}

std::ostream &operator<<(std::ostream &o, const vec3 &v)
{
    o << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return o;
}
