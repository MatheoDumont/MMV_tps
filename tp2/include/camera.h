#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>

#include "vec.h"

class Camera
{    
public:
    Camera();
    void lookAt(const vec3& pmin, const vec3& pmax);
    void rotation(const float x, const float y);
    void translation(const float x, const float y);
    void move(const float z);

    QMatrix4x4 view() const;
    QMatrix4x4 projection(const int width, const int height, const float fov) const;
    QVector3D position() const;

private:
    vec2 m_position;
    vec2 m_rotation;
    float m_size;
    float m_radius;

    vec3 m_center;
    
    QMatrix4x4 m_view;
};

#endif /* CAMERA_H */
