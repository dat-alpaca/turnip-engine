#version 460
layout (location = 0) out vec4 out_color;

layout (location = 0) in vec3 v_normal;
layout (location = 1) in vec2 v_uvs;
layout (location = 2) flat in uint v_textureID;

layout (binding = 3) uniform sampler2D u_textures[100];

void main()
{
	out_color = texture(u_textures[v_textureID], v_uvs);
}