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
const float shininess = 4.0;
const vec3 specularColor = vec3(0.5, 0.5, 0.5);

void main()
{
    vec3 o = normalize(u_camera - v_position);
    vec3 l = normalize(u_source - v_position);

    vec3 n = normalize(v_normal);

    // Phong
    float cos_theta = dot(n, l);
    float kpi = K / PI;
    // vec3 color = v_color * kpi *  cos_theta;
    
    // Blinn
    vec3 h = normalize(o + l);
    float cos_theta_h = dot(n, h);
    float fr = (1 - K) * (shininess + 8.0) / (8.0 * PI) * pow(cos_theta_h, shininess);

    vec3 color = (specularColor * fr + v_color * kpi) *  cos_theta;
   
    color = pow(color, vec3(GAMMA));
    
    frag_color = vec4(color, 1.f); 
}
