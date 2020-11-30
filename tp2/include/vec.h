#ifndef VEC_H
#define VEC_H

#include <iostream>

struct vec3
{
    double x;
    double y;
    double z;

    vec3() : x(0), y(0), z(0) {}

    vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    vec3(float x_, float y_) : x(x_), y(y_), z(0.) {}

    bool operator==(const vec3 &o) const;
};

// vec3 vec2(float x_, float y_)
// {
//     return vec3(x_, y_), 0.;
// }

typedef vec3 vec2;

vec3 operator-(const vec3 &p1, const vec3 &p2);

vec3 operator*(const vec3 &p1, const vec3 &p2);
vec3 operator*(float k, const vec3 &p);
vec3 operator*(const vec3 &p, float k);

vec3 operator/(const vec3 &p1, const vec3 &p2);
vec3 operator/(const vec3 &p, float k);
// vec3 operator/(float k, const vec3 &p);

vec3 operator+(const vec3 &p1, const vec3 &p2);

float norm(const vec3 &p);
vec3 normalize(const vec3 &p);
float dot(const vec3 &p1, const vec3 &p2);
vec3 cross(const vec3 &p1, const vec3 &p2);

std::ostream &operator<<(std::ostream &o, const vec3 &p);

#endif