#version 450 core
layout(location = 0) out vec4 out_color;

layout(location = 0) in vec2 v_uvs;
layout(location = 1) in vec3 v_normal;

layout(binding = 1) uniform sampler2D u_diffuse_texture;

void main()
{
	out_color = vec4(v_uvs, v_normal.x, 1.0);
	out_color = texture(u_diffuse_texture, v_uvs);
}