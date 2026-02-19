#version 450 core
layout(location = 0) out vec4 out_color;

layout(location = 0) in vec2 v_uvs;
layout(location = 1) flat in uint v_layer;
layout(location = 2) in vec4 v_color;

layout(binding = 2) uniform sampler2DArray u_texture;

void main() 
{
    float alpha = texture(u_texture, vec3(v_uvs.xy, float(v_layer))).r;

    if (alpha < 0.5)
        discard;
    
    out_color = v_color;
}