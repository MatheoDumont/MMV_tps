#ifndef VEC_H
#define VEC_H

#include <iostream>

struct vec2
{
    double x, y;

    vec2();
    explicit vec2(double a);
    vec2(double _x, double _y);
    vec2(const vec2& v);

    double length() const;
    vec2 normalize() const;

    vec2& operator=(const vec2 &rhs);
    bool operator==(const vec2 &rhs) const;
};

struct vec3
{
    double x, y, z;

    vec3();
    explicit vec3(double a);
    vec3(double _x, double _y, double _z);
    vec3(const vec3& v);
    vec3(const vec2& v, double _z);

    double length() const;
    vec3 normalize() const;

    vec3& operator=(const vec3 &rhs);
    bool operator==(const vec3 &rhs) const;
};

vec2 operator+(double k, const vec2 &v);
vec2 operator+(const vec2 &v, double k);
vec2 operator+(const vec2 &v1, const vec2 &v2);
vec2 operator-(const vec2 &v);
vec2 operator-(const vec2 &v1, const vec2 &v2);
vec2 operator*(double k, const vec2 &v);
vec2 operator*(const vec2 &v, double k);
vec2 operator*(const vec2& v1, const vec2 &v2);
vec2 operator/(const vec2& v, double k);

vec3 operator+(double k, const vec3 &v);
vec3 operator+(const vec3 &v, double k);
vec3 operator+(const vec3 &v1, const vec3 &v2);
vec3 operator-(const vec3 &v);
vec3 operator-(const vec3 &v1, const vec3 &v2);
vec3 operator*(double k, const vec3 &v);
vec3 operator*(const vec3 &v, double k);
vec3 operator*(const vec3 &v1, const vec3 &v2);
vec3 operator/(const vec3 &v, double k);


double dot(const vec2 &v1, const vec2 &v2);
double dot(const vec3 &v1, const vec3 &v2);

vec3 cross(const vec3 &v1, const vec3 &v2);

std::ostream &operator<<(std::ostream &o, const vec2 &v);
std::ostream &operator<<(std::ostream &o, const vec3 &v);

#endif
