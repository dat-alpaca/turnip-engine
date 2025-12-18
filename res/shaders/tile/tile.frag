#version 450 core
layout(location = 0) out vec4 out_color;

layout(location = 0) flat in uint v_layer;

void main() 
{
    out_color = vec4(1.0 / float(v_layer));
}