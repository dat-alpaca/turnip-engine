#version 450 core
layout(location = 0) out vec4 out_color;
layout(binding = 2)  uniform sampler2DArray u_tile_array;

layout(location = 0) in vec2 v_uv;
layout(location = 1) flat in uint v_layer;

void main() 
{
    out_color = texture(u_tile_array, vec3(v_uv, float(v_layer)));
}