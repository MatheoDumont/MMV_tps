#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;

uniform mat4 u_mvp;

out vec3 v_color;

void main()
{
    v_color = color;
    gl_Position = u_mvp * vec4(position, 1.f);
}
