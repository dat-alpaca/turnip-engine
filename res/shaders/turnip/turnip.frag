#version 460
#extension GL_EXT_nonuniform_qualifier : require

layout (location = 0) out vec4 out_color;

layout (location = 0) in vec3 v_normal;
layout (location = 1) in vec2 v_uvs;
layout (location = 2) flat in uint v_textureID;

layout (binding = 3) uniform sampler2D u_textures[];

void main()
{
	out_color = texture(u_textures[nonuniformEXT(v_textureID)], v_uvs);
}