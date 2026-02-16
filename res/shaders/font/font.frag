#version 450 core
layout(location = 0) out vec4 out_color;

layout(location = 0) in vec2 v_uvs;
layout(location = 1) flat in uint v_layer;

layout(binding = 2) uniform sampler2DArray u_texture;


void main() 
{
    float alpha = texture(u_texture, vec3(v_uvs.xy, float(v_layer))).r;
    vec3 color = vec3(1.0, 0.0, 1.0);
    out_color = vec4(color, alpha);
}