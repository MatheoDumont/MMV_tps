#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;

uniform mat4 u_mvp;

out vec3 v_position;
out vec3 v_color;
out vec3 v_normal;

void main()
{
    v_position = position;
    v_color = color;
    v_normal = normal;
    gl_Position = u_mvp * vec4(position, 1.f);
}
