#version 460
layout (location = 0) out vec4 out_color;

layout (location = 0) in vec2 v_uvs;
layout (location = 1) in vec3 v_normals;

void main()
{
    out_color = vec4(v_normals.yxz, 1.0);
}