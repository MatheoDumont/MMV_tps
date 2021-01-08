#include "camera.h"

#include <algorithm>

Camera::Camera() {}

void Camera::lookAt(const vec3 &pmin, const vec3 &pmax)
{
    vec3 middle;
    middle = (pmin + pmax) / 2.0;
    
    m_center = middle;
    m_position = vec2(0.0, 0.0);
    m_rotation = vec2(0.0, 180.0);
    m_size = (pmin - pmax).length();
    m_radius = m_size;
}

void Camera::rotation(const float x, const float y)
{
    m_rotation = m_rotation + vec2(y, x);
}

void Camera::translation(const float x, const float y)
{
    m_position.x -= m_size * x;
    m_position.y += m_size * y;
}

void Camera::move(const float z)
{
    m_size -= m_size * 0.01f * z;
    if (m_size < 0.01f)
        m_size = 0.01f;
}

QMatrix4x4 Camera::view() const
{
    QMatrix4x4 t;

    t.translate(-m_position.x, -m_position.y, -m_size);
    t.rotate(m_rotation.x, QVector3D(1, 0, 0));
    t.rotate(m_rotation.y, QVector3D(0, 1, 0));
    t.translate(-m_center.x, -m_center.y, -m_center.z);
    
    return t;
}

QMatrix4x4 Camera::projection(const int width, const int height, const float fov) const
{
    QMatrix4x4 p;
    float d = (vec3(m_position.x, m_position.y, m_size) - m_center).length();

    p.perspective(fov, float(width) / float(height),
                  std::max(0.1f, d - m_radius),
                  std::max(1.f, d + m_radius));
    return p;
}

QVector3D Camera::position() const
{
    QMatrix4x4 t = view();
    t.inverted();

    return t * QVector3D(0, 0, 0);
}
