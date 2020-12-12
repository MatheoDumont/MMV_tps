#version 330

in vec3 v_position;
in vec3 v_color;
in vec3 v_normal;

uniform vec3 u_camera;
uniform vec3 u_source;

out vec4 frag_color;

const float K = 0.9;
const float PI = 3.14159265359;
const float GAMMA = 1.0 / 2.2;

void main()
{
    vec3 o = normalize(u_camera - v_position);
    vec3 l = normalize(u_source - v_position);

    float cos_theta = dot(normalize(v_normal), l);
    float fr = K / PI;
  
    vec3 color = fr * v_color * cos_theta;
    color = pow(color, vec3(GAMMA));
    
    frag_color = vec4(color, 1.f);
}
