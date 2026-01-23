#version 450 core
layout(location = 0) out vec4 out_color;
layout(location = 0) in vec3 v_uvs;

layout(binding = 1) uniform samplerCube u_cubemap;

void main()
{
	out_color = texture(u_cubemap, v_uvs);
}